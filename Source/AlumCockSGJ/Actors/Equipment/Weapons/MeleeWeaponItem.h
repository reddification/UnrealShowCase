#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquippableItem.h"
#include "Data/MeleeAttackData.h"
#include "MeleeWeaponItem.generated.h"

UCLASS(Blueprintable)
class ALUMCOCKSGJ_API AMeleeWeaponItem : public AEquippableItem
{
	GENERATED_BODY()

public:
	
	const FMeleeAttackData* GetMeleeAttackData(EMeleeAttackType AttackType) const { return Attacks.Find(AttackType); }

	void SetIsHitRegistrationEnabled(bool bEnabled);
	void StartAttack(EMeleeAttackType AttackType, AController* AttackerController);
	void StopAttack();
	const FMeleeAttackData* GetActiveAttack() const { return ActiveAttack; }

	virtual void OnEquipped(UCharacterEquipmentComponent* CharacterEquipmentComponent) override;
	virtual void OnUnequipped(UCharacterEquipmentComponent* CharacterEquipmentComponent) override;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EMeleeAttackType, FMeleeAttackData> Attacks;
	
private:
	void OnMeleeHitRegistered(const FHitResult& HitResult, const FVector& Direction);
	
	TArray<class UMeleeHitRegistratorComponent*> HitRegistrators;
	TSet<AActor*> HitActors;

	const FMeleeAttackData* ActiveAttack = nullptr;
	AController* AttackerController;
};
