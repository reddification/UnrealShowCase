#include "BaseCharacterAnimInstance.h"

#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Characters/BaseCharacter.h"
#include "Components/Character/CharacterEquipmentComponent.h"
#include "Components/Combat/CharacterCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBaseCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	APawn* Pawn = TryGetPawnOwner();
	checkf(Pawn->IsA<ABaseCharacter>(), TEXT("UGCBaseCharacterAnimInstance can be used only with an AGCBaseCharacter derivative"))
	BaseCharacter = StaticCast<ABaseCharacter*>(Pawn);
}

void UBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!BaseCharacter.IsValid())
		return;
	
	const auto MovementComponent = BaseCharacter->GetCharacterMovement();
	Speed = MovementComponent->Velocity.Size();
	bInAir = MovementComponent->IsFlying() || MovementComponent->IsFalling();
	bStrafing = !MovementComponent->bOrientRotationToMovement;
	if (bStrafing)
	{
		Direction = CalculateDirection(MovementComponent->Velocity, BaseCharacter->GetActorRotation());
	}
	
	EquippedItemType = BaseCharacter->GetEquipmentComponent()->GetEquippedItemType();
	// Rotation = BaseCharacter->GetControlRotation();
	// Rotation.Pitch = Rotation.Pitch > 180 ? Rotation.Pitch - 360 : Rotation.Pitch;
	Rotation = BaseCharacter->GetAimOffset();
	const auto CurrentRangeWeapon = BaseCharacter->GetEquipmentComponent()->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		EEquippableItemType EquippableItemType = CurrentRangeWeapon->GetEquippableItemType();
		bForegrip = EquippableItemType == EEquippableItemType::TwoHandedRanged;
	}
	else
	{
		bForegrip = false;
	}

	// TODO works like shit. refactor/redo with virtual bones/2 hands ik?
	if (bForegrip)
	{
		WeaponForegripTransform = CurrentRangeWeapon->GetForegripTransform();
	}
	
	bAiming = BaseCharacter->GetCombatComponent()->IsAiming();
}

void UBaseCharacterAnimInstance::ApplyState(const UBaseNpcStateSettings* NpcState)
{
}
