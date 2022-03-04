#pragma once

#include "CoreMinimal.h"
#include "Data/EquipmentTypes.h"
#include "BaseCharacterAnimInstance.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void ApplyState(const class UBaseNpcStateSettings* NpcState);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Speed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bStrafing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = -180, UIMax = 180))
	float Direction = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEquippableItemType EquippedItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Transient, Category = "Character")
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Transient, Category = "Character")
	FTransform WeaponForegripTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Transient, Category = "Character")
	bool bForegrip;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Transient, Category = "Character")
	bool bAiming;

	TWeakObjectPtr<class ABaseCharacter> BaseCharacter;
};
