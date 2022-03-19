#include "NpcActivityStayInLocationInstance.h"

#include "CommonConstants.h"
#include "Actors/Interactive/Environment/QuestLocation.h"
#include "AI/Interfaces/ActivityCharacterProvider.h"

bool UNpcActivityStayInLocationInstance::FindNextLocation(const AAIController* AIController,
                                                          FNextLocationData& OutNextLocationData)
{
	if (!CoreLocation)
		return false;

	OutNextLocationData.NextLocation = CoreLocation->GetActorLocation();
	return true;
}

bool UNpcActivityStayInLocationInstance::AssessActivityState(AAIController* AIController,
	UBlackboardComponent* Blackboard)
{
	Blackboard->SetValueAsBool(BB_BoolParameter1, true);
	bAtLocation = true;
	FinalLocation = AIController->GetPawn()->GetActorLocation();
	return true;
}

void UNpcActivityStayInLocationInstance::RestoreBlackboardStateInternal(UBlackboardComponent* Blackboard)
{
	Super::RestoreBlackboardStateInternal(Blackboard);
	const float DistThreshold = 200.f;
	auto Character = Cast<IActivityCharacterProvider>(GetOuter())->GetActivityCharacter();
	if (bAtLocation && FinalLocation != FVector::ZeroVector)
		bAtLocation = FVector::DistSquared(Character->GetActorLocation(), FinalLocation) < (DistThreshold * DistThreshold);
	else
	{
		bAtLocation = false;
		FinalLocation = FVector::ZeroVector;
	}
	
	Blackboard->SetValueAsBool(BB_BoolParameter1, bAtLocation);
}
