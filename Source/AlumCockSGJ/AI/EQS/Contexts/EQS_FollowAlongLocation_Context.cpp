#include "EQS_FollowAlongLocation_Context.h"

#include "AIController.h"
#include "AI/Interfaces/NpcActivityManager.h"
#include "AI/Interfaces/NpcFollowingActivity.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Prediction.h"

void UEQS_FollowAlongLocation_Context::ProvideContext(FEnvQueryInstance& QueryInstance,
                                                      FEnvQueryContextData& ContextData) const
{
	// Super::ProvideContext(QueryInstance, ContextData);
	// auto Pawn = Cast<APawn>(QueryInstance.Owner.Get());
	// if (!Pawn)
	// 	return;
	//
	// auto AIController = Cast<AAIController>(Pawn->GetController());
	// if (!AIController)
	// 	return;
	//
	// TArray<FVector> PredictedLocations;
	// const FAISenseID SenseID = UAISense::GetSenseID(UAISense_Prediction::StaticClass());
	// for (auto DataIt = AIController->GetPerceptionComponent()->GetPerceptualDataConstIterator(); DataIt; ++DataIt)
	// {
	// 	if (!DataIt.Value().Target.IsValid())
	// 		continue;
	//
	// 	FVector PredictedLocation = DataIt->Value.GetStimulusLocation(SenseID);
	// 	if (PredictedLocation != FAISystem::InvalidLocation)
	// 		PredictedLocations.Add(PredictedLocation);
	// }
	//
	// UEnvQueryItemType_Point::SetContextHelper(ContextData, PredictedLocations);

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

	auto PredictedLocation = FollowingActivity->GetPredictedLocation();
	if (PredictedLocation != FAISystem::InvalidLocation)
		UEnvQueryItemType_Point::SetContextHelper(ContextData, PredictedLocation);
}
