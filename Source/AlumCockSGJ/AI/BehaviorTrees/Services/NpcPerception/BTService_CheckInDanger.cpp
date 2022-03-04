#include "BTService_CheckInDanger.h"

#include "AIController.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "AI/Interfaces/NpcController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_CheckInDanger::UBTService_CheckInDanger()
{
	NodeName = "Check if in danger";
	OutKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_CheckInDanger, OutKey));
}

void UBTService_CheckInDanger::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	auto PerceptiveController = Cast<INpcController>(OwnerComp.GetAIOwner());
	if (!PerceptiveController)
		return;

	bool bInDanger = PerceptiveController->IsInDanger();
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(OutKey.SelectedKeyName, bInDanger);
}

FString UBTService_CheckInDanger::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s\n%s"), *Super::GetStaticDescription(), *OutKey.SelectedKeyName.ToString());
}
