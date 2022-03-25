#pragma once

#include "NpcReactionTypes.generated.h"

UENUM()
enum class EReactionFeelingType : uint8
{
	Anxiety,
	Interest
};

UENUM(BlueprintType)
enum class EReactionSource : uint8
{
	None,
	Sound,
	Sight,
	Damage,
	Propagation
};

USTRUCT()
struct FReactionEvent
{
	GENERATED_BODY()

	UPROPERTY()
	const AActor* EventActor = nullptr;
	
	float ExpiresAt = 0.f;
	float FeelingChange = 0.f;
	FVector EventLocation = FVector::ZeroVector;
	// EReactionSource ReactionSource;
};

USTRUCT()
struct FPropagatedFeelingsData
{
	GENERATED_BODY()

	float FeelingLevel = 0.f;

	TMap<EReactionSource, TArray<FReactionEvent>> PropagatedReactions;
	FVector PropagatorLocation;
};

UENUM(BlueprintType)
enum ENpcState
{
	Anxious,
	Interested,
	Working,
	Resting
};

UENUM(BlueprintType)
enum class EFeelingState : uint8
{
	Absent = 0,
	Mild = 1,
	Average = 2,
	Extreme = 3
};

USTRUCT(BlueprintType)
struct FFeelingActivationThreshold
{
	GENERATED_BODY()

public:
	// Everything below this level is this state
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin = 0.f, ClampMax = 1.f, UIMin = 0.f, UIMax = 1.f))
	float Threshold = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EFeelingState State = EFeelingState::Absent;
};

struct FNpcReactResult
{
	bool bStarted = false;
	int ReactionIndex = -1;
	float ExpectedDuration = 0.f;
};