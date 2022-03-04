#include "BTTask_FaceLocation.h"

#include "AIController.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Characters/BaseCharacter.h"

UBTTask_FaceLocation::UBTTask_FaceLocation()
{
	NodeName = "Face Location";
	LookAtLocationKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FaceLocation, LookAtLocationKey), AActor::StaticClass());
	LookAtLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FaceLocation, LookAtLocationKey));
	bNotifyTick = 1;
}

EBTNodeResult::Type UBTTask_FaceLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FLookAtNodeMemory* Memory = (FLookAtNodeMemory*)NodeMemory;
	auto Pawn = OwnerComp.GetAIOwner()->GetPawn();
	auto Npc = Cast<INpcCharacter>(Pawn);
	if (!Npc)
		return EBTNodeResult::Failed;
	
	Memory->LookAtLocation = LookAtLocationKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass()
		? OwnerComp.GetBlackboardComponent()->GetValueAsVector(LookAtLocationKey.SelectedKeyName)
		: Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(LookAtLocationKey.SelectedKeyName))->GetActorLocation();
	
	FVector LookAtDirection = Memory->LookAtLocation - Pawn->GetActorLocation();
	Memory->PrecisionDot = FMath::Cos(FMath::DegreesToRadians(PrecisionDegrees));
	const float AngleDifference = CalculateAngleDifferenceDot(Pawn->GetActorForwardVector(), LookAtDirection);

	if (AngleDifference >= Memory->PrecisionDot)
		return EBTNodeResult::Succeeded;
	
	FRotator LookAtRotation = LookAtDirection.ToOrientationRotator();
	LookAtRotation.Pitch = 0.f;
	return Npc->SetAiDesiredRotation(LookAtRotation) ? EBTNodeResult::InProgress : EBTNodeResult::Failed;
}

void UBTTask_FaceLocation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (AIController == NULL || AIController->GetPawn() == NULL)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	else
	{
		FLookAtNodeMemory* Memory = (FLookAtNodeMemory*)NodeMemory;
		const FVector PawnDirection = AIController->GetPawn()->GetActorForwardVector();				

		float AngleDot = CalculateAngleDifferenceDot(PawnDirection, Memory->LookAtLocation - AIController->GetPawn()->GetActorLocation());
		if (AngleDot > Memory->PrecisionDot || FMath::IsNearlyEqual(AngleDot, Memory->PrecisionDot, KINDA_SMALL_NUMBER))
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

FString UBTTask_FaceLocation::GetStaticDescription() const
{
	return FString::Printf(TEXT("Look at %s"), *LookAtLocationKey.SelectedKeyName.ToString());
}

void UBTTask_FaceLocation::InitializeFromAsset(UBehaviorTree& Asset)
{
	// fuck unreal engine for shit like this how the fuck was i even supposed to know i have to do this to access selected key type fuck those bitches dickheads
	Super::InitializeFromAsset(Asset);
	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (BBAsset)
		LookAtLocationKey.ResolveSelectedKey(*BBAsset);
}

float UBTTask_FaceLocation::CalculateAngleDifferenceDot(const FVector& VectorA, const FVector& VectorB)
{
	return (VectorA.IsNearlyZero() || VectorB.IsNearlyZero())
		? 1.f
		: VectorA.CosineAngle2D(VectorB);
}
