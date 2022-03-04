#include "Actors/Equipment/Weapons/RangeWeaponItem.h"

#include "FMODBlueprintStatics.h"
#include "CommonConstants.h"
#include "Camera/CameraComponent.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Components/Combat/CharacterCombatComponent.h"
#include "Components/Combat/WeaponBarrelComponent.h"
#include "Data/DataAssets/Items/RangeWeaponSettings.h"
#include "Data/DataAssets/Items/WeaponBarrelSettings.h"
#include "Kismet/GameplayStatics.h"

ARangeWeaponItem::ARangeWeaponItem()
{
    WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
    WeaponMeshComponent->SetupAttachment(RootComponent);

    PrimaryWeaponBarrelComponent = CreateDefaultSubobject<UWeaponBarrelComponent>(TEXT("PrimaryBarrel"));
    PrimaryWeaponBarrelComponent->SetupAttachment(WeaponMeshComponent, MuzzleSocketName);

    ScopeCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Scope"));
    ScopeCameraComponent->SetupAttachment(WeaponMeshComponent);

    FmodAudioComponent = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("Audio"));
    FmodAudioComponent->SetupAttachment(WeaponMeshComponent);
}

void ARangeWeaponItem::BeginPlay()
{
    Super::BeginPlay();
    checkf(IsValid(ItemSettings) && ItemSettings->IsA<URangeWeaponSettings>(),
        TEXT("Item settings must be a valid RangeWeaponSettings DataAsset"));
    RangeWeaponSettings = StaticCast<const URangeWeaponSettings*>(ItemSettings);
    TInlineComponentArray<UWeaponBarrelComponent*> BarrelComponents;
    GetComponents<UWeaponBarrelComponent>(BarrelComponents);
    for (auto BarrelComponent : BarrelComponents)
    {
        UWeaponBarrelComponent* Barrel = StaticCast<UWeaponBarrelComponent*>(BarrelComponent);
        Barrel->SetAmmo(Barrel->GetWeaponBarrelSettings()->ClipCapacity);
        Barrels.Add(Barrel);
    }

    ActiveBarrelIndex = 0;
    ActiveWeaponBarrel = Barrels[ActiveBarrelIndex];
    AmmoChangedEvent.ExecuteIfBound(ActiveWeaponBarrel->GetAmmo());
}

bool ARangeWeaponItem::TryAddToEquipment(UCharacterEquipmentComponent* EquipmentComponent, const FPickUpItemData& PickUpItemData)
{
	bool bAdded = Super::TryAddToEquipment(EquipmentComponent, PickUpItemData);
	if (!bAdded)
		return false;

    EquipmentComponent->CharacterOwner->GetCombatComponent()->OnWeaponPickedUp(this);
    if (PickUpItemData.bDropped)
        SetAmmo(PickUpItemData.InitialAmmo);
    
    AmmoChangedEvent.BindUObject(EquipmentComponent, &UCharacterEquipmentComponent::OnAmmoChanged);
    OutOfAmmoEvent.BindUObject(EquipmentComponent, &UCharacterEquipmentComponent::OnOutOfAmmo);
    if (EquipmentComponent->EquipmentSettings->bAutoEquipNewItem || ItemSettings->DesignatedSlot == EquipmentComponent->EquippedSlot)
        EquipmentComponent->EquipItem(ItemSettings->DesignatedSlot);
    
    return true;
}

void ARangeWeaponItem::OnEquipped(UCharacterEquipmentComponent* CharacterEquipmentComponent)
{
    Super::OnEquipped(CharacterEquipmentComponent);
    CharacterEquipmentComponent->EquippedMeleeWeapon.Reset();
    CharacterEquipmentComponent->EquippedRangedWeapon = this;
    if (CanAim())
        CharacterEquipmentComponent->AimingSpeedChangedEvent.ExecuteIfBound(GetAimMaxSpeed());
    else
        CharacterEquipmentComponent->AimingSpeedResetEvent.ExecuteIfBound();

    CharacterEquipmentComponent->OnActiveWeaponBarrelChanged();
    AmmoChangedEvent.ExecuteIfBound(GetAmmo());
}

void ARangeWeaponItem::OnUnequipped(UCharacterEquipmentComponent* CharacterEquipmentComponent)
{
    Super::OnUnequipped(CharacterEquipmentComponent);
    CharacterEquipmentComponent->EquippedRangedWeapon.Reset();
}

void ARangeWeaponItem::OnDropped(APickableEquipmentItem* PickableEquipmentItem)
{
    PickableEquipmentItem->SetDroppedState(GetAmmo());
}

EReticleType ARangeWeaponItem::GetReticleType() const
{
    return bAiming ? ActiveWeaponBarrel->GetWeaponBarrelSettings()->AimReticleType : RangeWeaponSettings->ReticleType;
}

#pragma region SHOOT

bool ARangeWeaponItem::TryStartFiring(AController* ShooterController)
{
    // why not bFiring?
    if (GetWorld()->GetTimerManager().IsTimerActive(ShootTimer))
    {
        return false;
    }

    CachedShooterController = ShooterController;
    bFiring = true;
    return Shoot();
}

void ARangeWeaponItem::StopFiring()
{
    bFiring = false;
}

void ARangeWeaponItem::PlayFmodEvent(UFMODEvent* SFX)
{
    FmodAudioComponent->Stop();
    FmodAudioComponent->SetEvent(SFX);
    FmodAudioComponent->Play();
}

bool ARangeWeaponItem::Shoot()
{
    int32 Ammo = GetAmmo();
    if (Ammo <= 0)
    {
        auto NoAmmoSFX = ActiveWeaponBarrel->GetWeaponBarrelSettings()->ShootWithNoAmmoSFX;
        if (IsValid(NoAmmoSFX))
        {
            PlayFmodEvent(NoAmmoSFX);
            // UFMODBlueprintStatics::PlayEventAttached(NoAmmoSFX, ActiveWeaponBarrel.Get(), NAME_None, FVector::ZeroVector,
            //     EAttachLocation::KeepRelativeOffset, true, true, true);
        }

        if (!bReloading)
        {
            OutOfAmmoEvent.ExecuteIfBound();
            StopFiring();
        }
        return false;
    }

    FVector ViewLocation;
    FRotator ViewRotation;
    CachedShooterController->GetPlayerViewPoint(ViewLocation, ViewRotation);
    FVector ViewDirection = ViewRotation.Vector();
    auto WeaponBarrelSettings = ActiveWeaponBarrel->GetWeaponBarrelSettings();
    for (auto i = 0; i < WeaponBarrelSettings->BulletsPerShot; i++)
        ActiveWeaponBarrel->Shoot(ViewLocation, ViewDirection + GetBulletSpreadOffset(ViewRotation), CachedShooterController);

    if (!ActiveWeaponBarrel->GetWeaponBarrelSettings()->bInfiniteClips)
        SetAmmo(Ammo - 1);

    PlayAnimMontage(WeaponBarrelSettings->WeaponShootMontage);
    auto PlayerController = Cast<APlayerController>(CachedShooterController);
    if (PlayerController)
        PlayerController->PlayerCameraManager->StartCameraShake(CamShake);
    
    ActiveWeaponBarrel->FinalizeShot();
    if (ShootEvent.IsBound())
        ShootEvent.Broadcast(WeaponBarrelSettings->CharacterShootMontage);

    GetWorld()->GetTimerManager().SetTimer(ShootTimer, this, &ARangeWeaponItem::ResetShot, GetShootTimerInterval(), false);
    return true;
}

void ARangeWeaponItem::ResetShot()
{
    if (!bFiring)
    {
        return;
    }

    switch (ActiveWeaponBarrel->GetWeaponBarrelSettings()->FireMode)
    {
    case EWeaponFireMode::Single:
        StopFiring();
        break;
    case EWeaponFireMode::FullAuto:
        Shoot();
        break;
    default:
        StopFiring();
        break;
    }
}

FVector ARangeWeaponItem::GetBulletSpreadOffset(const FRotator& ShotOrientation) const
{
    float PitchAngle = FMath::RandRange(0.f, GetBulletSpreadAngleRad());
    if (FMath::IsNearlyZero(PitchAngle, KINDA_SMALL_NUMBER))
    {
        return FVector::ZeroVector;
    }

    float SpreadSize = FMath::Tan(PitchAngle);
    float RotationAngle = FMath::RandRange(0.f, 2 * PI);
    float SpreadY = FMath::Cos(RotationAngle);
    float SpreadZ = FMath::Sin(RotationAngle);

    return (ShotOrientation.RotateVector(FVector::UpVector) * SpreadZ + ShotOrientation.RotateVector(FVector::RightVector) * SpreadY) *
           SpreadSize;
}

float ARangeWeaponItem::GetBulletSpreadAngleRad() const
{
    const auto WeaponBarrelSettings = ActiveWeaponBarrel->GetWeaponBarrelSettings();
    return FMath::DegreesToRadians(bAiming ? WeaponBarrelSettings->AimSpreadAngle : WeaponBarrelSettings->SpreadAngle);
}

#pragma endregion SHOOT

#pragma region AIM

bool ARangeWeaponItem::StartAiming()
{
    if (CanAim() && !bAiming)
    {
        bAiming = true;
        return true;
    }

    return false;
}

void ARangeWeaponItem::StopAiming()
{
    bAiming = false;
}

#pragma endregion AIM

#pragma region RELOAD

void ARangeWeaponItem::StartReloading(float DesiredReloadDuration)
{
    bReloading = true;
    PlayAnimMontage(ActiveWeaponBarrel->GetWeaponBarrelSettings()->WeaponReloadMontage, DesiredReloadDuration);
}

void ARangeWeaponItem::StopReloading(bool bInterrupted)
{
    bReloading = false;
    if (bInterrupted)
    {
        WeaponMeshComponent->GetAnimInstance()->Montage_Stop(0.0f, ActiveWeaponBarrel->GetWeaponBarrelSettings()->WeaponReloadMontage);
    }
}

#pragma endregion RELOAD

#pragma region FIRE MODES

void ARangeWeaponItem::StartTogglingFireMode()
{
    bChangingFireMode = true;
}

void ARangeWeaponItem::CompleteTogglingFireMode()
{
    ActiveBarrelIndex = (ActiveBarrelIndex + 1) % Barrels.Num();
    ActiveWeaponBarrel = Barrels[ActiveBarrelIndex];
    AmmoChangedEvent.ExecuteIfBound(ActiveWeaponBarrel->GetAmmo());
    bChangingFireMode = false;
}

const UWeaponBarrelSettings* ARangeWeaponItem::GetNextWeaponBarrelSettings() const
{
    UWeaponBarrelComponent* NextBarrel = Barrels[(ActiveBarrelIndex + 1) % Barrels.Num()];
    return NextBarrel->GetWeaponBarrelSettings();
}

#pragma endregion FIRE MODES

void ARangeWeaponItem::SetAmmo(int32 NewAmmo)
{
    ActiveWeaponBarrel->SetAmmo(NewAmmo);
    AmmoChangedEvent.ExecuteIfBound(NewAmmo);
}

FTransform ARangeWeaponItem::GetForegripTransform() const
{
    return WeaponMeshComponent->GetSocketTransform(SocketForegrip);
}

float ARangeWeaponItem::PlayAnimMontage(UAnimMontage* AnimMontage, float DesiredDuration)
{
    if (!IsValid(AnimMontage))
        return -1.f;

    const float PlayRate = DesiredDuration > 0 ? AnimMontage->GetPlayLength() / DesiredDuration : 1;
    const auto AnimInstance = WeaponMeshComponent->GetAnimInstance();
    if (IsValid(AnimInstance))
    {
        return AnimInstance->Montage_Play(AnimMontage, PlayRate);
    }

    return -1.f;
}
