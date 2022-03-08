#pragma once

#include "CoreMinimal.h"
#include "Data/EquipmentTypes.h"
#include "CharacterCombatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FAimingStateChangedEvent, bool bAiming, class ARangeWeaponItem* Weapon)

UCLASS()
class ALUMCOCKSGJ_API UCharacterCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void StartFiring();
	void StopFiring();

	void StartPrimaryMeleeAttack();
	void StopPrimaryMeleeAttack();
	void StartHeavyMeleeAttack();
	void StopHeavyMeleeAttack();
	void StartMeleeAttack(EMeleeAttackType AttackType, AController* AttackerController);
	void StopMeleeAttack(EMeleeAttackType AttackType);
	void OnMeleeAttackCompleted();

	void TryThrow();
	void GrabThrowableItem();
	void InterruptThrowingItem();
	void ActivateThrowableItem() const;
	void ReleaseThrowableItem();

	bool CanThrow() const;
	
	void StartAiming();
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
