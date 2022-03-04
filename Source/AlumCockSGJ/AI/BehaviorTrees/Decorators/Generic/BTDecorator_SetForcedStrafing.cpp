#include "BTDecorator_SetForcedStrafing.h"

#include "AIController.h"
#include "AI/Interfaces/NpcCharacter.h"

UBTDecorator_SetForcedStrafing::UBTDecorator_SetForcedStrafing()
{
	NodeName = "Set forced strafing";
	bNotifyActivation = 1;
	bNotifyDeactivation = 1;
}

void UBTDecorator_SetForcedStrafing::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
	Super::OnNodeActivation(SearchData);
	auto NpcCharacter = Cast<INpcCharacter>(SearchData.OwnerComp.GetAIOwner()->GetPawn());
	if (NpcCharacter)
	{
		auto Memory = GetNodeMemory<FBTMemory_ForcedStrafing>(SearchData);
		Memory->bInitialForceStrafing = NpcCharacter->IsAiForcingStrafing();
		NpcCharacter->SetAiForcedStrafing(bForceStrafing);
	}
}

void UBTDecorator_SetForcedStrafing::OnNodeDeactivation(FBehaviorTreeSearchData& SearchData,
	EBTNodeResult::Type NodeResult)
{
	Super::OnNodeDeactivation(SearchData, NodeResult);
	auto NpcCharacter = Cast<INpcCharacter>(SearchData.OwnerComp.GetAIOwner()->GetPawn());
	if (NpcCharacter)
	{
		auto Memory = GetNodeMemory<FBTMemory_ForcedStrafing>(SearchData);
		NpcCharacter->SetAiForcedStrafing(Memory->bInitialForceStrafing);
	}
}

FString UBTDecorator_SetForcedStrafing::GetStaticDescription() const
{
	return FString::Printf(TEXT("Force strafing: %s"), bForceStrafing ? TEXT("true") : TEXT("false"));
}

uint16 UBTDecorator_SetForcedStrafing::GetInstanceMemorySize() const
{
	return sizeof(FBTMemory_ForcedStrafing);
}
