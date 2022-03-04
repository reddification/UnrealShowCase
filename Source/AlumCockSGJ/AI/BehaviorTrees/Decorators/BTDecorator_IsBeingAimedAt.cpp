#include "BTDecorator_IsBeingAimedAt.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BaseHumanoidCharacter.h"

UBTDecorator_IsBeingAimedAt::UBTDecorator_IsBeingAimedAt()
{
    NodeName = "Is being aimed at";
    CharacterKey.AddObjectFilter(this,
        GET_MEMBER_NAME_CHECKED(UBTDecorator_IsBeingAimedAt, CharacterKey), AActor::StaticClass());
}

bool UBTDecorator_IsBeingAimedAt::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!IsValid(AIController))
        return false;
    
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    const ABaseCharacter* Character = Cast<ABaseCharacter>(Blackboard->GetValueAsObject(CharacterKey.SelectedKeyName));
    if (!IsValid(Character))
        return false;

    FVector CharacterLocation;
    FRotator CharacterViewRotation;

    // TODO dirty hack find out the real problem
    if (!IsValid(Character->Controller))
        return false;
    
    Character->Controller->GetPlayerViewPoint(CharacterLocation, CharacterViewRotation);
    FVector CharacterToBotDirection = (AIController->GetPawn()->GetActorLocation() - Character->GetActorLocation()).GetSafeNormal();
    float dp = FVector::DotProduct(CharacterToBotDirection, CharacterViewRotation.Vector());
    bool bAimedAt = dp > DotProductThreshold;
    return FVector::DotProduct(CharacterToBotDirection, CharacterViewRotation.Vector()) > DotProductThreshold;
}
