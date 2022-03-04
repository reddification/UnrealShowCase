#include "BTService_AimAdjustment.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/Character.h"

UBTService_AimAdjustment::UBTService_AimAdjustment()
{
    NodeName = "Adjust aim location";
    CharacterKey.AddObjectFilter(this,
        GET_MEMBER_NAME_CHECKED(UBTService_AimAdjustment, CharacterKey), ACharacter::StaticClass());
}

void UBTService_AimAdjustment::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
    auto Character = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (!!Character)
        return;

    AAIController* AIController = OwnerComp.GetAIOwner();
    FVector AITargetLocation = Character->GetMesh()->GetSocketLocation(AISocketDefaultCharacterTarget);
    if (!AITargetLocation.IsZero())
        AIController->SetFocalPoint(AITargetLocation);
}
