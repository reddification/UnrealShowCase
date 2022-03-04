#pragma once

#include "NpcActivityTypes.generated.h"

USTRUCT()
struct FActivityIgnoreActorData
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Actor = nullptr;

	float UntilWorldTime = 0.f;
};

UENUM(BlueprintType)
enum class ENpcActivityLatentActionState : uint8
{
	Completed,
	Started,
	Failed,
	Aborted
};

UENUM(BlueprintType)
enum class EActivityEventType : uint8
{
	Iteration,
	Assessment,
	ArbitraryFailure,
	Inoperable, // i.e. when there are no items to interact
	Completed,
	Timeout
};

UENUM()
enum class ETraversalType
{
	Sequential = 0,
	Closest = 1,
	Random = 2,
};

UENUM(BlueprintType)
enum class ENpcActivityType : uint8
{
	None,
	Rest,
	Work,
	Quest,
};
