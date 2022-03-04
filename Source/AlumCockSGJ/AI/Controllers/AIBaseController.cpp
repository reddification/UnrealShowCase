#include "AIBaseController.h"

#include "BrainComponent.h"
#include "AI/Components/Controller/Activities/AIBaseActivityComponent.h"
#include "AI/Components/Controller/Reactions/AIBaseReactionComponent.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Prediction.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Team.h"
#include "Perception/AISense_Touch.h"

FName AAIBaseController::ActivitiesComponentName(TEXT("Activities"));
FName AAIBaseController::ReactionsComponentName(TEXT("Reactions"));

AAIBaseController::AAIBaseController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
    ActivitiesComponent = CreateDefaultSubobject<UAIBaseActivityComponent>(AAIBaseController::ActivitiesComponentName);
    ReactionsComponent = CreateDefaultSubobject<UAIBaseReactionComponent>(AAIBaseController::ReactionsComponentName);
}

void AAIBaseController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (IsValid(InPawn))
	{
	    checkf(InPawn->IsA<ABaseCharacter>(), TEXT("AAICharacterController is intended to be used only with ABaseCharacter pawn"))
		ControlledCharacter = StaticCast<ABaseCharacter*>(InPawn);
		ControlledCharacter->GetBaseCharacterAttributesComponent()->OutOfHealthEvent.AddUObject(this, &AAIBaseController::OnDeath);
	}
}

void AAIBaseController::SetInterestingLocation(const FVector& Location)
{
    Blackboard->SetValueAsVector(BB_InterestingLocation, Location);
}

void AAIBaseController::SetDefaultNavigationQueryFilterClass(const TSubclassOf<UNavigationQueryFilter>& OverridingNavigationQueryFilterClass)
{
	DefaultNavigationFilterClass = OverridingNavigationQueryFilterClass;
}

void AAIBaseController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (ControllerPawnResetEvent.IsBound())
		ControllerPawnResetEvent.Broadcast();
}

void AAIBaseController::OnDeath()
{
	if (IsValid(PerceptionComponent))
	{
		PerceptionComponent->SetSenseEnabled(UAISense_Sight::StaticClass(), false);
		PerceptionComponent->SetSenseEnabled(UAISense_Damage::StaticClass(), false);
		PerceptionComponent->SetSenseEnabled(UAISense_Hearing::StaticClass(), false);
		PerceptionComponent->SetSenseEnabled(UAISense_Prediction::StaticClass(), false);
		PerceptionComponent->SetSenseEnabled(UAISense_Touch::StaticClass(), false);
		PerceptionComponent->SetSenseEnabled(UAISense_Team::StaticClass(), false);
	}

	if (IsValid(BrainComponent))
		BrainComponent->StopLogic("Dead");
	
    UnPossess();
}
