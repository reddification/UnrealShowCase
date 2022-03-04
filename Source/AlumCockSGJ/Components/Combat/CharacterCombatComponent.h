#pragma once

#include "CoreMinimal.h"
#include "Data/EquipmentTypes.h"
#include "UObject/Object.h"
#include "CharacterCombatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FAimingStateChangedEvent, bool bAiming, class ARangeWeaponItem* Weapon)

UCLASS()
class ALUMCOCKSGJ_API UCharacterCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void StartShooting(AController* Controller) const;
	void StopFiring() const;

	void StartPrimaryMeleeAttack(AController* AttackerController);
	void StopPrimaryMeleeAttack();
	void StartHeavyMeleeAttack(AController* AttackerController);
	void StopHeavyMeleeAttack();
	void StartMeleeAttack(EMeleeAttackType AttackType, AController* AttackerController);
	void StopMeleeAttack(EMeleeAttackType AttackType);
	void OnMeleeAttackCompleted();

	bool TryThrow();
	void GrabThrowableItem();
	void InterruptThrowingItem();
	void ActivateThrowableItem() const;
	void ReleaseThrowableItem();

	bool CanThrow() const;
	
	bool StartAiming();
	void StopAiming();
	bool IsMeleeAttacking() const { return bMeleeAttack; }
	bool IsAiming() const { return bAiming; }

	void SetMeleeHitRegEnabled(bool bEnabled);
	void OnWeaponPickedUp(ARangeWeaponItem* RangeWeapon);
	mutable FAimingStateChangedEvent AimStateChangedEvent;
	
protected:
	virtual void BeginPlay() override;

	// The smaller this value the faster melee attacks are
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Melee")
	float MeleeDurationMultiplier = 1.f;
	
private:
	TWeakObjectPtr<class ABaseCharacter> CharacterOwner;
	bool bThrowing = false;
	bool bAiming = false;
	bool bMeleeAttack = false;
	void OnShot(UAnimMontage* Montage);
	FTimerHandle ThrowTimer;
	FTimerHandle MeleeAttackTimer;
};
