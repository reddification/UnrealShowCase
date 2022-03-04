#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NpcActivityBaseSettings.h"
#include "NpcBehaviorSettings.generated.h"

USTRUCT(BlueprintType)
struct FNpcActivityUtilitySettings
{
	GENERATED_BODY()

	// Absolute value
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Utility", meta=(UIMin=0.f, UIMax=1.f, ClampMin = 0.f, ClampMax = 1.f))
	float InitialUtility = 1.f;
	
	// Absolute value
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Utility", meta=(UIMin=0.f, UIMax=1.f, ClampMin = 0.f, ClampMax = 1.f))
	float IterationUtilityDecay = 0.1f;

	// Absolute value
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Utility", meta=(UIMin=0.f, UIMax=1.f, ClampMin = 0.f, ClampMax = 1.f))
	float AssessmentUtilityDecay = 0.1f;

	// Absolute value
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Utility", meta=(UIMin=0.f, UIMax=1.f, ClampMin = 0.f, ClampMax = 1.f))
	float ArbitraryFailureUtilityDecay = 0.3f;

	// In seconds
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Utility", meta=(UIMin=0.0001f, ClampMin = 0.0001f))
	float UtilityRestoreInterval = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Utility", meta=(UIMin=0.f, UIMax=1.f, ClampMin = 0.f, ClampMax = 1.f))
	float UtilityRestoreGain = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Energy", meta=(UIMin=-1.f, UIMax=1.f, ClampMin = -1.f, ClampMax = 1.f))
	float IterationEnergyChange = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Energy", meta=(UIMin=-1.f, UIMax=1.f, ClampMin = -1.f, ClampMax = 1.f))
	float AssessmentEnergyChange = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Energy", meta=(UIMin=-1.f, UIMax=1.f, ClampMin = -1.f, ClampMax = 1.f))
	float FailEnergyChange = 0.1f;
};

USTRUCT(BlueprintType)
struct FNpcActivityEnergyRestoreSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Energy", meta=(UIMin=0.1f, ClampMin = 0.1f))
	float EnergyRestoreInterval = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Energy", meta=(UIMin=0.f, UIMax=1.f, ClampMin = 0.f, ClampMax = 1.f))
	float EnergyRestoreGain = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boredom", meta=(UIMin=0.1f, ClampMin = 0.1f))
	float BoredomAccumulationInterval = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boredom", meta=(UIMin=0.f, UIMax=1.f, ClampMin = 0.f, ClampMax = 1.f))
	float BoredomTimedIncrease = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boredom", meta=(UIMin=0.f, UIMax=1.f, ClampMin = 0.f, ClampMax = 1.f))
	float BoredomIncreaseOnFail = 0.3f;
};

USTRUCT(BlueprintType)
struct FNpcBehaviorActivityDescriptor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UBehaviorTree* BehaviorTree;

	// Activity's internal type class
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UNpcActivityInstanceBase> ActivityInstanceClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNpcActivityBaseSettings* NpcActivitySettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin = 0.f, UIMin = 0.f))
	float WorkUtility = 0.2f;
};

USTRUCT(BlueprintType)
struct FNpcBehaviorReactionDescriptor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag BehaviorTag;
};

USTRUCT(BlueprintType)
struct FNpcActivityWorkBehavior : public FNpcBehaviorActivityDescriptor
{
	GENERATED_BODY()
	
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin=0, ClampMin=0, UIMax=23, ClampMax=23))
	// int HoursFrom;
	//
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin=0, ClampMin=0, UIMax=23, ClampMax=23))
	// int HoursTo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FNpcActivityUtilitySettings ActivityUtilitySettings;
};

USTRUCT(BlueprintType)
struct FNpcActivityRestBehavior : public FNpcBehaviorActivityDescriptor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FNpcActivityEnergyRestoreSettings ActivityEnergyRestoreSettings;
};