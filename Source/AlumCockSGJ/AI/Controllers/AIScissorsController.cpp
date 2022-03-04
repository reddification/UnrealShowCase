#include "AIScissorsController.h"

#include "AI/Characters/AICharacter.h"
#include "AI/Components/Controller/Activities/AIBasicActivityComponent.h"
#include "AI/Components/Controller/Reactions/AIEnemyReactionsComponent.h"
#include "Perception/AIPerceptionComponent.h"

AAIScissorsController::AAIScissorsController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer
        .SetDefaultSubobjectClass<UAIBasicActivityComponent>(AAIBaseController::ActivitiesComponentName)
        .SetDefaultSubobjectClass<UAIEnemyReactionsComponent>(AAIBaseController::ReactionsComponentName))
{
}

void AAIScissorsController::BeginPlay()
{
    Super::BeginPlay();
}

void AAIScissorsController::OnTargetKilled(const AActor* Actor)
{
    if (FocusedCharacter == Actor)
        FocusedCharacter.Reset();

    if (TargetKilledEvent.IsBound())
        TargetKilledEvent.Broadcast(Actor);
}

void AAIScissorsController::ResetFocusedCharacter()
{
    FocusedCharacter.Reset();
}

void AAIScissorsController::ReportKill(const AActor* KilledActor)
{
    OnTargetKilled(KilledActor);
}

FVector AAIScissorsController::GetFocalPointOnActor(const AActor* Actor) const
{
    if(FocusedCharacter.IsValid())
    {
        FVector AimAtLocation = FocusedCharacter->GetMesh()->GetSocketLocation(DefaultAimSocket);
        if (AimAtLocation != FVector::ZeroVector)
            return AimAtLocation;
    }
    
    return Super::GetFocalPointOnActor(Actor);
}

void AAIScissorsController::SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority)
{
    Super::SetFocus(NewFocus, InPriority);
    FocusedCharacter = Cast<ABaseCharacter>(NewFocus);
}

void AAIScissorsController::GetPlayerViewPoint(FVector& Location, FRotator& Rotation) const
{
    Super::GetPlayerViewPoint(Location, Rotation);
    if (FocusedCharacter.IsValid())
    {
        FVector AimAtLocation = GetFocalPointOnActor(FocusedCharacter.Get());
        Rotation = (AimAtLocation - Location).Rotation();
    }
}
