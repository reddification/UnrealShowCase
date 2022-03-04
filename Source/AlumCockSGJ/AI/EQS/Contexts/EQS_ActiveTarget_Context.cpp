#include "AI/EQS/Contexts/EQS_ActiveTarget_Context.h"
#include "CommonConstants.h"
#include "AI/Controllers/AIBaseController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEQS_ActiveTarget_Context::ProvideContext(FEnvQueryInstance& QueryInstance,
                                               FEnvQueryContextData& ContextData) const
{
	APawn* QuerierPawn = Cast<APawn>(QueryInstance.Owner.Get());
	if (!IsValid(QuerierPawn))
		return;
	
	AActor* ResultingActor = NULL;
	AAIController* AIController = Cast<AAIController>(QuerierPawn->GetController());
	if (IsValid(AIController))
	{
		const auto Blackboard = AIController->GetBlackboardComponent();
		if (IsValid(Blackboard))
		    ResultingActor = Cast<AActor>(Blackboard->GetValueAsObject(BB_CurrentTarget));
	}
	
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, ResultingActor);
}
