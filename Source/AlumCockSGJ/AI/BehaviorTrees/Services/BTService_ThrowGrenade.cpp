#include "BTService_ThrowGrenade.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Components/Combat/CharacterCombatComponent.h"

UBTService_ThrowGrenade::UBTService_ThrowGrenade()
{
	NodeName = "Throw grenade";
}

void UBTService_ThrowGrenade::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	AAIController* Controller = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!IsValid(Controller) || !IsValid(Blackboard))
	{
		return;
	}
	
	auto BotCharacter = Cast<ABaseCharacter>(Controller->GetPawn());
	if (!IsValid(BotCharacter))
	{
		return;
	}

	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!IsValid(CurrentTarget) || !BotCharacter->GetCombatComponent()->CanThrow())
		return;

	float DistSq = FVector::DistSquared(BotCharacter->GetActorLocation(), CurrentTarget->GetActorLocation());
	if (!FMath::IsWithin(DistSq, MinAttackRange * MinAttackRange, MaxAttackRange * MaxAttackRange))
	{
		return;
	}
	
	BotCharacter->ThrowItem();
}
