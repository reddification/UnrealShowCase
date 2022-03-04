#include "BTTask_AssessActivityState.h"

#include "CommonConstants.h"
#include "AI/ActivityInstances/ActivityInstancesHelper.h"
#include "AI/Interfaces/AssessableActivity.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Data/LogTypes.h"

UBTTask_AssessActivityState::UBTTask_AssessActivityState()
{
	NodeName = "Assess activity state"; // and fill blackboard values
}

EBTNodeResult::Type UBTTask_AssessActivityState::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
                                                                      uint8* NodeMemory)
{
	auto ActivityState = GetActivityInstance(OwnerComp);
	if (!ActivityState)
		return EBTNodeResult::Failed;

	auto AssessableActivity = Cast<IAssessableActivity>(ActivityState);
	if (!AssessableActivity)
		return EBTNodeResult::Failed;

	bool bAssessedSuccessfully = AssessableActivity->AssessActivityState(OwnerComp.GetAIOwner(), OwnerComp.GetBlackboardComponent());
	return bAssessedSuccessfully ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
