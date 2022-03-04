#include "BTService_LookForActor.h"
#include "AIController.h"
#include "GameplayTagAssetInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"

UBTService_LookForActor::UBTService_LookForActor()
{
	NodeName = "Look for actor";
	OutActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_LookForActor, OutActorKey), AActor::StaticClass());
}

void UBTService_LookForActor::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
		return;
	
	TArray<AActor*> ObservedActors;
	AIController->GetPerceptionComponent()->GetKnownPerceivedActors(UAISense_Sight::StaticClass(), ObservedActors);
	for (const auto ObservedActor : ObservedActors)
	{
		if (ObservedActor->IsA(ActorClassToLookFor))
		{
			if (WithTagsQuery.IsEmpty())
			{
				AIController->GetBlackboardComponent()->SetValueAsObject(OutActorKey.SelectedKeyName, ObservedActor);
				return;
			}
			else 
			{
				IGameplayTagAssetInterface* GameplayTagActor = Cast<IGameplayTagAssetInterface>(ObservedActor);
				if (!GameplayTagActor)
					return; // return because currently only 1 type of actor is supported

				FGameplayTagContainer ActorGameplayTags;
				GameplayTagActor->GetOwnedGameplayTags(ActorGameplayTags);
				if (WithTagsQuery.Matches(ActorGameplayTags))
				{
					AIController->GetBlackboardComponent()->SetValueAsObject(OutActorKey.SelectedKeyName, ObservedActor);
					return;
				}
			}
		}
	}
}

FString UBTService_LookForActor::GetStaticDescription() const
{
	FString ResultDescription = Super::GetStaticDescription();
	if (IsValid(ActorClassToLookFor))
		ResultDescription = ResultDescription.Append(FString::Printf(TEXT("\nActor type: %s"), *ActorClassToLookFor.Get()->GetName()));

	if (!WithTagsQuery.IsEmpty())
		ResultDescription = ResultDescription.Append(FString::Printf(TEXT("\nWith tags: %s"), *WithTagsQuery.GetDescription()));

	if (OutActorKey.IsSet())
		ResultDescription = ResultDescription.Append(FString::Printf(TEXT("\nOut BB Key: %s"), *OutActorKey.SelectedKeyName.ToString()));
	
	return ResultDescription;
}

