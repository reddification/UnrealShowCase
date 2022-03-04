#pragma once

#include "PatrolSettings.generated.h"

UENUM(BlueprintType)
enum class EPatrolType : uint8
{
	Circle,
	PingPong
};

UENUM(BlueprintType)
enum class EPatrolMode : uint8
{
	Defensive = 0,
	Aggressive = 1
};

USTRUCT(BlueprintType)
struct FPatrolSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class APatrolPath* PatrolPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPatrolType PatrolType = EPatrolType::Circle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPatrolMode PatrolMode = EPatrolMode::Defensive;
};