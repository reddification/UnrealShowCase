#pragma once
#include "AI/Data/AITypesGC.h"
#include "Data/CharacterTypes.h"

#include "BaseCharacterDataAsset.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBaseCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name;
	
	// X - absolute value of Velocity.Z, Y - Fall damage 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hard Land")
	UCurveFloat* FallDamageCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hard Land")
	UAnimMontage* HardLandMontageTP = nullptr;

	// Velocity.Z at which HardLand starts occuring
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hard Land")
	float HardLandVelocityZ = 2000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animations")
	TArray<UAnimMontage*> HitReactionMontages;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animations")
	UAnimMontage* DeathAnimationMontage;

	// List of actions that block key action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action filters")
	TMap<ECharacterAction, FCharacterActions> ActionBlockers;

	// List of actions that are interrupted by key action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action filters")
	TMap<ECharacterAction, FCharacterActions> ActionInterrupters;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Bones-Sockets")
	FName HeadBoneName = FName("head");
};