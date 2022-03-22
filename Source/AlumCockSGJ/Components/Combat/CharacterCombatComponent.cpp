#include "CharacterCombatComponent.h"
#include "Actors/Equipment/EquippableItem.h"
#include "Actors/Equipment/Weapons/MeleeWeaponItem.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Actors/Equipment/Weapons/ThrowableItem.h"
#include "Actors/Projectiles/BaseThrowableProjectile.h"
#include "Characters/BaseCharacter.h"
#include "Components/Character/CharacterEquipmentComponent.h"

void UCharacterCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	CharacterOwner = Cast<ABaseCharacter>(GetOwner());
}

void UCharacterCombatComponent::StartAiming()
{
	auto EquippedRangedWeapon = CharacterOwner->GetEquipmentComponent()->GetCurrentRangeWeapon();
	if (!IsValid(EquippedRangedWeapon) || !EquippedRangedWeapon->CanAim() || !CharacterOwner->CanStartAction(ECharacterAction::Aim))
		return;

	EquippedRangedWeapon->StartAiming();
	if (AimStateChangedEvent.IsBound())
		AimStateChangedEvent.Broadcast(true, EquippedRangedWeapon);

	bAiming = true;
	CharacterOwner->OnActionStarted(ECharacterAction::Aim);
}

void UCharacterCombatComponent::StopAiming()
{
	auto EquippedRangedWeapon = CharacterOwner->GetEquipmentComponent()->GetCurrentRangeWeapon();
	CharacterOwner->OnActionEnded(ECharacterAction::Aim);
	if (!bAiming)
		return;
	
	bAiming = false;
	if (!IsValid(EquippedRangedWeapon))
		return;

	EquippedRangedWeapon->StopAiming();
	if (AimStateChangedEvent.IsBound())
		AimStateChangedEvent.Broadcast(false, EquippedRangedWeapon);
}

void UCharacterCombatComponent::StartFiring()
{
	auto Controller = CharacterOwner->GetController();
	auto EquippedRangedWeapon = CharacterOwner->GetEquipmentComponent()->GetCurrentRangeWeapon();
	if (IsValid(EquippedRangedWeapon) && CharacterOwner->CanStartAction(ECharacterAction::Shoot))
	{
		bool bStarted = EquippedRangedWeapon->TryStartFiring(Controller);
		if (bStarted)
		{
			CharacterOwner->OnActionStarted(ECharacterAction::Shoot);
		}
	}
}

void UCharacterCombatComponent::StopFiring()
{
	auto EquippedRangedWeapon = CharacterOwner->GetEquipmentComponent()->GetCurrentRangeWeapon();
	if (IsValid(EquippedRangedWeapon) && EquippedRangedWeapon->IsFiring())
	{
		EquippedRangedWeapon->StopFiring();
		CharacterOwner->OnActionEnded(ECharacterAction::Shoot);
	}
}

void UCharacterCombatComponent::OnShot(UAnimMontage* Montage)
{
	CharacterOwner->PlayAnimMontage(Montage);
}

bool UCharacterCombatComponent::CanThrow() const
{
	auto ActiveThrowable = CharacterOwner->GetEquipmentComponent()->GetCurrentThrowable();
	return IsValid(ActiveThrowable)
		? CharacterOwner->CharacterEquipmentComponent->GetActiveThrowablesCount() > 0
		: false;
}

void UCharacterCombatComponent::TryThrow()
{
	if (bThrowing || !CharacterOwner->CanStartAction(ECharacterAction::ThrowItem) || !CanThrow())
		return;
	
	bThrowing = true;
	CharacterOwner->OnActionStarted(ECharacterAction::ThrowItem);
	auto ActiveThrowable = CharacterOwner->GetEquipmentComponent()->GetCurrentThrowable();
	UAnimMontage* ThrowMontage = ActiveThrowable->GetThrowMontage();
	if (IsValid(ThrowMontage))
	{
		PreThrowCollisionEnabledType = CharacterOwner->GetMesh()->GetCollisionEnabled();
		CharacterOwner->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		const float PlayRate = ThrowMontage->GetPlayLength() / ActiveThrowable->GetThrowDuration();
		CharacterOwner->GetEquipmentComponent()->PutActiveItemInSecondaryHand();
		CharacterOwner->PlayAnimMontage(ThrowMontage, PlayRate);
		GetWorld()->GetTimerManager().SetTimer(ResetCollisionTimer, this, &UCharacterCombatComponent::ResetCharacterCollisionType,
			ActiveThrowable->GetThrowDuration() * PlayRate);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(ThrowTimer, this, &UCharacterCombatComponent::ReleaseThrowableItem,
			ActiveThrowable->GetThrowDuration());
	}
}

void UCharacterCombatComponent::GrabThrowableItem()
{
	if (!bThrowing)
		return;

	auto ActiveThrowable = CharacterOwner->GetEquipmentComponent()->GetCurrentThrowable();
	auto CurrentProjectile = ActiveThrowable->SpawnProjectile();
	CurrentProjectile->AttachToComponent(CharacterOwner->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform,
		ActiveThrowable->GetCharacterEquippedSocketName());

	CharacterOwner->GetEquipmentComponent()->OnThrowableUsed();
}

void UCharacterCombatComponent::InterruptThrowingItem()
{
	CharacterOwner->OnActionEnded(ECharacterAction::ThrowItem);
	auto ActiveThrowable = CharacterOwner->GetEquipmentComponent()->GetCurrentThrowable();
	if (!bThrowing || !IsValid(ActiveThrowable))
		return;
	
	if (ActiveThrowable->GetThrowMontage())
	{
		CharacterOwner->StopAnimMontage(ActiveThrowable->GetThrowMontage());
	}
	
	GetWorld()->GetTimerManager().ClearTimer(ThrowTimer);
	ActiveThrowable->DropProjectile(CharacterOwner->GetController());
	bThrowing = false;
	CharacterOwner->GetEquipmentComponent()->PutActiveItemInPrimaryHand();
}

void UCharacterCombatComponent::ActivateThrowableItem() const
{
	auto ActiveThrowable = CharacterOwner->GetEquipmentComponent()->GetCurrentThrowable();
	if (IsValid(ActiveThrowable))
		ActiveThrowable->Activate(CharacterOwner->GetController());
}

void UCharacterCombatComponent::ReleaseThrowableItem()
{
	auto ActiveThrowable = CharacterOwner->GetEquipmentComponent()->GetCurrentThrowable();
	if (!bThrowing || !IsValid(ActiveThrowable))
	{
		CharacterOwner->OnActionEnded(ECharacterAction::ThrowItem);
		return;
	}

	ActiveThrowable->Throw(CharacterOwner->GetController());
	bThrowing = false;

	CharacterOwner->GetEquipmentComponent()->PutActiveItemInPrimaryHand();
	CharacterOwner->OnActionEnded(ECharacterAction::ThrowItem);
}

void UCharacterCombatComponent::StartPrimaryMeleeAttack()
{
	auto AttackerController = CharacterOwner->GetController();
	StartMeleeAttack(EMeleeAttackType::Primary, AttackerController);
}

void UCharacterCombatComponent::StopPrimaryMeleeAttack()
{
	StopMeleeAttack(EMeleeAttackType::Primary);
}

void UCharacterCombatComponent::StartHeavyMeleeAttack()
{
	auto AttackerController = CharacterOwner->GetController();
	StartMeleeAttack(EMeleeAttackType::Secondary, AttackerController);
}

void UCharacterCombatComponent::StopHeavyMeleeAttack()
{
	StopMeleeAttack(EMeleeAttackType::Secondary);
}

void UCharacterCombatComponent::StartMeleeAttack(EMeleeAttackType AttackType, AController* AttackerController)
{
	auto EquippedMeleeWeapon = CharacterOwner->GetEquipmentComponent()->GetCurrentMeleeWeapon();
	if (!IsValid(EquippedMeleeWeapon) || bMeleeAttack || !CharacterOwner->CanStartAction(ECharacterAction::MeleeAttack))
	{
		return;
	}

	const FMeleeAttackData* AttackData = EquippedMeleeWeapon->GetMeleeAttackData(AttackType);
	if (!IsValid(AttackData->Montage) || CharacterOwner->CharacterAttributesComponent->GetStamina() < AttackData->StaminaConsumption)
	{
		return;
	}
	
	bMeleeAttack = true;
	CharacterOwner->OnActionStarted(ECharacterAction::MeleeAttack);
	EquippedMeleeWeapon->StartAttack(AttackType, AttackerController);
	CharacterOwner->CharacterAttributesComponent->ChangeAttribute(ECharacterAttribute::Stamina, -AttackData->StaminaConsumption);
	float Duration = AttackData->DesiredDuration * MeleeDurationMultiplier;
	if (AttackData->Montage->HasRootMotion())
	{
		CharacterOwner->bUseControllerRotationYaw = true;	
	}
	
	CharacterOwner->PlayAnimMontageWithDuration(AttackData->Montage, Duration);
	GetWorld()->GetTimerManager().SetTimer(MeleeAttackTimer, this, &UCharacterCombatComponent::OnMeleeAttackCompleted, Duration);
}

void UCharacterCombatComponent::StopMeleeAttack(EMeleeAttackType AttackType)
{
	auto EquippedMeleeWeapon = CharacterOwner->GetEquipmentComponent()->GetCurrentMeleeWeapon();
	if (!IsValid(EquippedMeleeWeapon) || !bMeleeAttack)
	{
		CharacterOwner->OnActionEnded(ECharacterAction::MeleeAttack);
		return;
	}

	const FMeleeAttackData* AttackData = EquippedMeleeWeapon->GetMeleeAttackData(AttackType);
	if (!AttackData || !AttackData->bChargeable)
	{
		return;
	}
	
	CharacterOwner->StopAnimMontage(AttackData->Montage);
	GetWorld()->GetTimerManager().ClearTimer(MeleeAttackTimer);
	OnMeleeAttackCompleted();
}

void UCharacterCombatComponent::OnMeleeAttackCompleted()
{
	auto EquippedMeleeWeapon = CharacterOwner->GetEquipmentComponent()->GetCurrentMeleeWeapon();
	bMeleeAttack = false;
	CharacterOwner->OnActionEnded(ECharacterAction::MeleeAttack);
	if (!IsValid(EquippedMeleeWeapon))
	{
		return;
	}
	
	const FMeleeAttackData* ActiveAttack = EquippedMeleeWeapon->GetActiveAttack();
	if (ActiveAttack && IsValid(ActiveAttack->Montage) && ActiveAttack->Montage->HasRootMotion())
	{
		CharacterOwner->UpdateStrafingControls();
	}
	
	if (IsValid(EquippedMeleeWeapon))
	{
		EquippedMeleeWeapon->StopAttack();
	}
}

void UCharacterCombatComponent::SetMeleeHitRegEnabled(bool bEnabled)
{
	auto EquippedMeleeWeapon = CharacterOwner->GetEquipmentComponent()->GetCurrentMeleeWeapon();
	if (IsValid(EquippedMeleeWeapon) && bMeleeAttack)
		EquippedMeleeWeapon->SetIsHitRegistrationEnabled(bEnabled);
}

void UCharacterCombatComponent::OnWeaponPickedUp(ARangeWeaponItem* RangeWeapon)
{
	RangeWeapon->ShootEvent.AddUObject(this, &UCharacterCombatComponent::OnShot);
}

void UCharacterCombatComponent::ResetCharacterCollisionType()
{
	CharacterOwner->GetMesh()->SetCollisionEnabled(PreThrowCollisionEnabledType);
}
