#include "BTService_ConversationSearch.h"

#include "AIController.h"
#include "AI/Components/Character/NpcConversationComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"

void UBTService_ConversationSearch::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	FConversationServiceMemory* ServiceMemory = (FConversationServiceMemory*)NodeMemory;
	if (ServiceMemory->Cooldown > 0.f)
	{
		ServiceMemory->Cooldown -= DeltaSeconds;
		return;
	}
	
	TArray<AActor*> PerceivedActors;
	auto AIController = OwnerComp.GetAIOwner();
	AIController->GetPerceptionComponent()->GetKnownPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
	if (PerceivedActors.Num() <= 0)
		return;

	auto Component = AIController->GetPawn()->GetComponentByClass(UNpcConversationComponent::StaticClass());
	if (!Component)
		return;

	auto CurrentConversationComponent = StaticCast<UNpcConversationComponent*>(Component);
	if (CurrentConversationComponent->IsInDialogue())
		return;
	
	for (const auto PerceivedActor : PerceivedActors)
	{
		Component = PerceivedActor->GetComponentByClass(UNpcConversationComponent::StaticClass());
		if (!Component)
			continue;

		float ActorsSqDistance = FVector::DistSquared(PerceivedActor->GetActorLocation(), AIController->GetPawn()->GetActorLocation());
		if (ActorsSqDistance > ConversationDistance * ConversationDistance)
			continue;

		if (FMath::RandRange(0.f, 1.f) > AttemptConversationProbability)
			continue;
		
		auto OtherConversationComponent = StaticCast<UNpcConversationComponent*>(Component);
		if (CurrentConversationComponent->RequestConversation(OtherConversationComponent))
		{
			ServiceMemory->Cooldown = ConversationAttemptCooldown;
			return;	
		}
	}
}
