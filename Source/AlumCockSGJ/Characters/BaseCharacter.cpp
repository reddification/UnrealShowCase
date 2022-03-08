#include "BaseCharacter.h"
#include "CommonConstants.h"
#include "FMODAudioComponent.h"
#include "NavigationSystem.h"
#include "Actors/Interactive/BaseWorldItem.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/Character/BaseCharacterAttributesComponent.h"
#include "Components/Character/CharacterEquipmentComponent.h"
#include "Components/Combat/CharacterCombatComponent.h"
#include "Components/Movement/BaseCharacterMovementComponent.h"
#include "Data/DataAssets/Characters/CharacterInteractionOptions.h"
#include "Data/Entities/WorldItemDTR.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	CharacterAttributesComponent = CreateDefaultSubobject<UBaseCharacterAttributesComponent>(CharacterAttributesComponentName);
	AddOwnedComponent(CharacterAttributesComponent);

	CharacterEquipmentComponent = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("Equipment"));
	AddOwnedComponent(CharacterEquipmentComponent);

	CharacterCombatComponent = CreateDefaultSubobject<UCharacterCombatComponent>(TEXT("Combat"));
	AddOwnedComponent(CharacterCombatComponent);

	BaseMovementComponent = StaticCast<UBaseCharacterMovementComponent*>(GetCharacterMovement());
	
	FmodAudioComponent = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("FmodAudioComponent"));
	FmodAudioComponent->SetupAttachment(GetMesh(), "head");
}

FCharacterInteractionParameters ABaseCharacter::GetInteractionParameters(const FGameplayTag& InteractionTag) const
{
	FCharacterInteractionParameters Result;
	UCharacterInteractionOptions* MainInteractionOptions = GetInteractionOptions();
	if (!MainInteractionOptions)
		return Result;

	auto Options = MainInteractionOptions->InteractionMontages.Find(InteractionTag);
	if (!Options || Options->Montages.Num() <= 0)
		return Result;
	
	Result.CharacterInteractionOptionIndex = FMath::RandRange(0, Options->Montages.Num() - 1);
	Result.InteractionMontage = Options->Montages[Result.CharacterInteractionOptionIndex];
	return Result;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
    checkf(IsValid(CharacterSettings), TEXT("Character settings DataAsset must be set"));
    CharacterAttributesComponent->OutOfHealthEvent.AddUObject(this, &ABaseCharacter::OnOutOfHealth);

    OnTakeAnyDamage.AddDynamic(CharacterAttributesComponent, &UBaseCharacterAttributesComponent::OnTakeAnyDamage);
    OnTakeAnyDamage.AddDynamic(this, &ABaseCharacter::ReactToDamage);
    CreateLoadout();
}

void ABaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	CharacterEquipmentComponent->DestroyEquipment();
}

bool ABaseCharacter::IsConsumingStamina() const
{
	return CharacterCombatComponent->IsMeleeAttacking();
}

bool ABaseCharacter::IsPreferStrafing() const
{
	return GetCharacterMovement()->IsMovingOnGround() && CharacterEquipmentComponent->IsPreferStrafing()
		&& !CharacterAttributesComponent->IsOutOfStamina();
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	IGenericTeamAgentInterface* AIController = Cast<IGenericTeamAgentInterface>(NewController);
	if (AIController)
	{
		AIController->SetGenericTeamId(FGenericTeamId((uint8)Team));
	}
}

void ABaseCharacter::OnLevelDeserialized_Implementation()
{
    
}

void ABaseCharacter::CreateLoadout()
{
    if (bIsLoadoutCreated==true)
    {
        return;
    }
    UE_LOG(LogTemp,Warning,TEXT("%s loadout created"),*GetNameSafe(this))
    CharacterEquipmentComponent->CreateLoadout();
    bIsLoadoutCreated=true;
}


void ABaseCharacter::OnOutOfHealth()
{
	bool bDeathMontagePlaying = false;
	// StopAnimMontage();
	if (GetMovementComponent()->IsMovingOnGround())
	{
		const float Duration = PlayAnimMontage(CharacterSettings->DeathAnimationMontage);
		bDeathMontagePlaying = Duration > 0.f;
	}
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	if (!bDeathMontagePlaying)
	{
		EnableRagdoll();
	}
	
	SpawnItemsOnDeath();
	if (CharacterDeathEvent.IsBound())
		CharacterDeathEvent.Broadcast(this);
}

void ABaseCharacter::MoveForward(float Value)
{
	CurrentInputForward = Value;
}

void ABaseCharacter::EnableRagdoll() const
{
	GetMesh()->SetCollisionProfileName(ProfileRagdoll);
	GetMesh()->SetSimulatePhysics(true);
}

float ABaseCharacter::PlayAnimMontageWithDuration(UAnimMontage* Montage, float DesiredDuration) const
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	const float PlayRate = Montage->GetPlayLength() / DesiredDuration;
	return AnimInstance->Montage_Play(Montage, PlayRate);
}

void ABaseCharacter::ReactToDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (!CharacterAttributesComponent->IsAlive())
	{
		return;
	}
	
	if (CharacterSettings->HitReactionMontages.Num() > 0)
	{
		UAnimMontage* HitReactionMontage = CharacterSettings->HitReactionMontages[FMath::RandRange(0, CharacterSettings->HitReactionMontages.Num() -1)];
		PlayAnimMontage(HitReactionMontage);	
	}

	OnActionStarted(ECharacterAction::ReceiveDamage);
	OnActionEnded(ECharacterAction::ReceiveDamage);
}

void ABaseCharacter::SetStrafingControlsState(bool bStrafing)
{
	GetCharacterMovement()->bOrientRotationToMovement = !bStrafing;
	bUseControllerRotationYaw = bStrafing;
}

bool ABaseCharacter::CanStartAction(ECharacterAction Action)
{
	if (!CharacterAttributesComponent->IsAlive())
	{
		return false;
	}
	
	const FCharacterActions* BlockedByActions = CharacterSettings->ActionBlockers.Find(Action);
	if (!BlockedByActions || BlockedByActions->AffectedActions.Num() == 0)
	{
		return true;
	}

	return BlockedByActions->AffectedActions.Intersect(ActiveActions).Num() == 0;
}

void ABaseCharacter::HandleCancellingActions(ECharacterAction ActionToInterrupt)
{
	switch (ActionToInterrupt)
	{
		case ECharacterAction::Aim:
			CharacterCombatComponent->StopAiming();
			break;
		case ECharacterAction::Reload:
			CharacterEquipmentComponent->InterruptReloading();
			break;
		case ECharacterAction::Shoot:
			CharacterCombatComponent->StopFiring();
			break;
		case ECharacterAction::ChangeEquipment:
			CharacterEquipmentComponent->InterruptChangingEquipment();
			break;
		case ECharacterAction::ThrowItem:
			CharacterCombatComponent->InterruptThrowingItem();
			break;
		default:
			// these are uninterruptable
			break;
	}
}

void ABaseCharacter::OnActionStarted(ECharacterAction Action)
{
	ActiveActions.Add(Action);
	const FCharacterActions* ActionsToInterrupt = CharacterSettings->ActionInterrupters.Find(Action);
	if (!ActionsToInterrupt)
		return;

	TSet<ECharacterAction> AffectedActions = ActionsToInterrupt->AffectedActions.Intersect(ActiveActions);
	for (ECharacterAction ActionToInterrupt : AffectedActions)
	{
		HandleCancellingActions(ActionToInterrupt);
	}
}

void ABaseCharacter::OnActionEnded(ECharacterAction Action)
{
	ActiveActions.Remove(Action);
}

void ABaseCharacter::UpdateStrafingControls()
{
	SetStrafingControlsState(IsPreferStrafing());
}

bool ABaseCharacter::SetDesiredRotation(const FRotator& Rotator)
{
	bUseControllerRotationYaw = 0;	
	BaseMovementComponent->SetDesiredRotation(Rotator);
	return true;
}

#pragma region WEAPONS

bool ABaseCharacter::CanShoot() const
{
	return !GetCharacterMovement()->IsSwimming();
}

void ABaseCharacter::StartFiring()
{
	if (CharacterAttributesComponent->IsAlive() && CanShoot())
	{
		CharacterCombatComponent->StartFiring();
	}
}

void ABaseCharacter::StopFiring()
{
	CharacterCombatComponent->StopFiring();
}

void ABaseCharacter::StartAiming()
{
	CharacterCombatComponent->StartAiming();
}

void ABaseCharacter::StopAiming()
{
	CharacterCombatComponent->StopAiming();
}

void ABaseCharacter::StartReloading()
{
	if (CanReload())
	{
		CharacterEquipmentComponent->TryReload();
	}
}

void ABaseCharacter::ChangeWeapon(int WeaponIndexDelta)
{
	CharacterEquipmentComponent->EquipItem(WeaponIndexDelta);
}

void ABaseCharacter::ToggleFireMode()
{
	CharacterEquipmentComponent->StartTogglingFireMode();
}

void ABaseCharacter::ThrowItem()
{
	CharacterCombatComponent->TryThrow();
}

bool ABaseCharacter::CanReload() const
{
	return GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling();
}

#pragma region MELEE

void ABaseCharacter::StartPrimaryMeleeAttack()
{
	CharacterCombatComponent->StartPrimaryMeleeAttack();
}

void ABaseCharacter::StopPrimaryMeleeAttack()
{
	CharacterCombatComponent->StopPrimaryMeleeAttack();
}

void ABaseCharacter::StartSecondaryMeleeAttack()
{
	CharacterCombatComponent->StartHeavyMeleeAttack();
}

void ABaseCharacter::StopSecondaryMeleeAttack()
{
	CharacterCombatComponent->StopHeavyMeleeAttack();
}

#pragma endregion MELEE

// TODO move away from character to subsystem
void ABaseCharacter::SpawnItems(const TArray<FSpawnItemDescription>& SpawnItemsList)
{
	if (SpawnItemsList.Num() <= 0)
		return;
	
	FVector NavLocation;
	for (const auto SpawnItemDescription : SpawnItemsList)
	{
		if (SpawnItemDescription.SpawnProbability < FMath::FRandRange(0.f, 1.f))
			continue;
		
		if (SpawnItemDescription.WorldItemDTRH.IsNull())
			continue;

		auto WorldItemDTR = SpawnItemDescription.WorldItemDTRH.GetRow<FWorldItemDTR>("");
		if (!WorldItemDTR)
			continue;
		
		bool bRandomLocationFound = UNavigationSystemV1::K2_GetRandomReachablePointInRadius(GetWorld(), GetActorLocation(), NavLocation, 75.f);
		ABaseWorldItem* SpawnedPickable = GetWorld()->SpawnActor<ABaseWorldItem>(WorldItemDTR->SpawnWorldItemClass,
			bRandomLocationFound ? NavLocation + FVector::UpVector * 100.f : GetActorLocation() + FVector::UpVector * 25.f, FRotator::ZeroRotator);

		if (!SpawnItemDescription.WithTags.IsEmpty())
			SpawnedPickable->ChangeGameplayTags(SpawnItemDescription.WithTags, true);
		
		SpawnedPickable->SetItemDTRH(SpawnItemDescription.WorldItemDTRH);
	}
}

void ABaseCharacter::SpawnItemsOnDeath()
{
	SpawnItems(SpawnItemsAfterDeath);
}

#pragma region TAGS

void ABaseCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = GameplayStateTags;
}

bool ABaseCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return GameplayStateTags.HasTagExact(TagToCheck);
}

bool ABaseCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return GameplayStateTags.HasAllExact(TagContainer);
}

bool ABaseCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return GameplayStateTags.HasAnyExact(TagContainer);
}

void ABaseCharacter::ChangeGameplayTags(const FGameplayTagContainer& TagsContainer, bool bAdd)
{
	if (bAdd)
		GameplayStateTags.AppendTags(TagsContainer);
	else
		GameplayStateTags.RemoveTags(TagsContainer);
	
	if (CharacterGameplayStateChangedEvent.IsBound())
		CharacterGameplayStateChangedEvent.Broadcast(GameplayStateTags);
}

float ABaseCharacter::PlayFmodEvent(UFMODEvent* FmodEvent)
{
	FmodAudioComponent->Stop();
	FmodAudioComponent->SetEvent(FmodEvent);
	FmodAudioComponent->Play();
	return FmodAudioComponent->GetLength() / 1000.f;
}

void ABaseCharacter::InteruptVoiceLine()
{
	FmodAudioComponent->Stop();
}

#pragma endregion TAGS

void ABaseCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	auto SocketTransform = GetMesh()->GetSocketTransform(SocketEyesViewpoint);
	OutLocation = SocketTransform.GetLocation();
	OutRotation = SocketTransform.Rotator();
}
