#pragma once

#include "BehaviorTree/BehaviorTree.h"
#include "AI/ActivityInstances/NpcActivityInstance.h"
#include "NpcBaseActivityWrapper.generated.h"

USTRUCT()
struct FNpcBaseActivityWrapper
{
	GENERATED_BODY()
	
	UPROPERTY()
	UBehaviorTree* BehaviorTree;
	
	UPROPERTY()
	UNpcActivityInstanceBase* ActivityInstance;

	float ActivityNecessity = 0.f;
};

USTRUCT()
struct FNpcRestActivityWrapper : public FNpcBaseActivityWrapper
{
	GENERATED_BODY()

	FNpcActivityEnergyRestoreSettings ActivityEnergyRestoreSettings;
};

USTRUCT()
struct FNpcWorkActivityWrapper : public FNpcBaseActivityWrapper
{
	GENERATED_BODY()

	FNpcActivityUtilitySettings ActivityUtilitySettings;
	float Utility = 0.f;
	int CompletedTimes = 0;
};

USTRUCT()
struct FNpcQuestActivityWrapper : public FNpcBaseActivityWrapper
{
	GENERATED_BODY()
};
