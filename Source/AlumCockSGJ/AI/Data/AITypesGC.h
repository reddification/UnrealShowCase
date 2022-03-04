#pragma once

#include "GenericTeamAgentInterface.h"
#include "AITypesGC.generated.h"

UENUM(BlueprintType)
enum class ETeam : uint8
{
	Player,
	GoodGuys,
	BadGuys,
	Dead
};

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Patrolling,
	Investigating,
	Pursuing,
	Attacking,
	Retreating,
	Covering,
	Cautious
};

UENUM(BlueprintType)
enum class EAICombatReason : uint8
{
    None,
    Visual,
    Defend
};

UENUM()
enum class EAIStimulusState
{
	Idle,
	SeeFriendlyWithGun,
	SeeEnemyUnreciprocal,
	HearInterestingSound,
	HearDangerousSound,
	SeeEnemyReciprocal,
};

inline ETeamAttitude::Type GCTeamAttitudeSolver(FGenericTeamId A, FGenericTeamId B)
{
	if (A.GetId() ==(uint8)ETeam::Dead || B.GetId() == (uint8)ETeam::Dead)
		return ETeamAttitude::Neutral;
	
	return A != B ? ETeamAttitude::Hostile : ETeamAttitude::Friendly;
}

