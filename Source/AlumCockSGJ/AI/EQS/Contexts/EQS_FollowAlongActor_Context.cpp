#include "EQS_FollowAlongActor_Context.h"

#include "AIController.h"
#include "AI/Interfaces/NpcActivityManager.h"
#include "AI/Interfaces/NpcFollowingActivity.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Prediction.h"

void UEQS_FollowAlongActor_Context::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);
	auto Owner = Cast<APawn>(QueryInstance.Owner);
	if (!Owner)
		return;
	
	auto ActivityManager = Cast<INpcActivityManager>(Owner->GetController());
	if (!ActivityManager)
		return;

	auto FollowingActivity = Cast<INpcFollowingActivity>(ActivityManager->GetRunningActivity());
	if (!FollowingActivity)
		return;

	auto FollowedActor = FollowingActivity->GetFollowedActor();
	if (FollowedActor)
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, FollowedActor);
}
