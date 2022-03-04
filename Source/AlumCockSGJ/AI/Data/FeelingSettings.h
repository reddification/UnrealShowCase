#pragma once
#include "NpcReactionTypes.h"
#include "GameplayTagContainer.h"
#include "FeelingSettings.generated.h"

UCLASS(BlueprintType)
class UFeelingSettings : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = 1, UIMax = 64, ClampMin = 1, ClampMax = 64))
	// int FeelingMemorySize = 8;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.001f, UIMin = 0.001f))
	float MaxFeelLevel = 2.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.001f, UIMin = 0.001f), Category="Feel change")
	float HearingShotFeelChange = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.001f, UIMin = 0.001f), Category="Feel change")
	float SeeingEnemyFeelChange = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.001f, UIMin = 0.001f), Category="Feel change")
	float SeeingEnemyReciprocallyFeelChange = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.001f, UIMin = 0.001f), Category="Feel change")
	float TakingDamageFeelChange = 0.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.001f, UIMin = 0.001f), Category="Feel change")
	float TakingDamageWhenSeeEnemyFeelChange = 0.8f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.001f, UIMin = 0.001f), Category="Feel change")
	float HearingExplosionFeelChange = 0.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.001f, UIMin = 0.001f), Category="Feel change")
	float WitnessDeadBodyFeelChange = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.001f, UIMin = 0.001f), Category="Feel change")
	TMap<FGameplayTag, float> PerceivedReactionFeelingChanges;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.5f, UIMin = 0.5f), Category="Hold feeling")
	float MaxHoldTime = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.1f, UIMin = 0.1f), Category="Hold feeling")
	float HoldDecayRate = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.1f, UIMin = 0.1f))
	float FeelingDecayRate = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.1f, UIMin = 0.1f))
	TMap<EReactionSource, float> ReactionMemoryLifetimes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FFeelingActivationThreshold> FeelingStateActivationThresholds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EReactionSource, UCurveFloat*> ReactionsCountToFeelingFactorDependency;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* RangeToFeelingFactorDependency = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* FeelingToHoldTimeDependency = nullptr;

	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	// UCurveFloat* ReactionsCountToFeelingFactorDependency = nullptr;

	// Normalized value +- added to feeling change on reaction event
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = 0.f, UIMax = 1.f, ClampMin = 0.f, ClampMax = 1.f))
	float FeelingDeviation = 0.1;
	
};

UCLASS(BlueprintType)
class UFeelingPropagationSettings : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.01f, ClampMax = 2.f, UIMin = 0.01f, UIMax = 2.f))
	float FeelingPropagationFactor = 0.8f;

	// Interval by which this NPC can receive feeling propagation from the same another NPC
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.01f, UIMin = 0.01f))
	float FeelingPropagationDelay = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin = 1, UIMin=1))
	int PropagatedReactionsCount = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.01f, UIMin = 0.01f))
	float MaxAnxietyPropagationDistance = 2500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.01f, UIMin = 0.01f))
	float MaxInterestPropagationDistance = 3500.f;

	// 0..1
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* InterestPropagationFactorDistanceDependency;

	// 0..1
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* AnxietyPropagationFactorDistanceDependency;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin = 1, UIMin=1))
	int PropagationMemorySize = 16;
};

USTRUCT(BlueprintType)
struct FNpcInteractWithPlayerAnxietyEffect
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin = 0.f, UIMin = 0.f))
	float WitnessPlayerCalmEffect = 0.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin = 0.f, UIMin = 0.f))
	float InteractWithPlayerCalmEffect = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin = 0.f, UIMin = 0.f))
	float IgnoreWitnessPlayerInterval = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin = 0.f, UIMin = 0.f))
	float IgnoreInteractWithPlayerInterval = 10.f;
};

UCLASS(BlueprintType)
class UNpcFeelingSettingsBundle : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = 1, UIMax = 64, ClampMin = 1, ClampMax = 64))
	int FeelingMemorySize = 12;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UFeelingSettings* AnxietySettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UFeelingSettings* InterestSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UFeelingPropagationSettings* FeelingPropagationSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = 1, ClampMin = 1))
	TMap<EReactionSource, int> ReactionSourceEndangermentThresholds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FNpcInteractWithPlayerAnxietyEffect NpcInteractWithPlayerAnxietyEffect;
};
