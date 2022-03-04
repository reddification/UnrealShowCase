#include "AIOutlawController.h"

#include "AI/Components/Controller/Activities/AIBasicActivityComponent.h"
#include "AI/Components/Controller/Reactions/AIEnemyReactionsComponent.h"
#include "Characters/BaseCharacter.h"

void AAIOutlawController::BeginPlay()
{
	Super::BeginPlay();
}

AAIOutlawController::AAIOutlawController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer
		.SetDefaultSubobjectClass<UAIBasicActivityComponent>(AAIBaseController::ActivitiesComponentName)
		.SetDefaultSubobjectClass<UAIEnemyReactionsComponent>(AAIBaseController::ReactionsComponentName))
{
}

void AAIOutlawController::OnTargetKilled(const AActor* Actor)
{
	if (FocusedCharacter == Actor)
		FocusedCharacter.Reset();

	if (TargetKilledEvent.IsBound())
		TargetKilledEvent.Broadcast(Actor);
}

void AAIOutlawController::ResetFocusedCharacter()
{
	FocusedCharacter.Reset();
}

void AAIOutlawController::ReportKill(const AActor* KilledActor)
{
	OnTargetKilled(KilledActor);
}

FVector AAIOutlawController::GetFocalPointOnActor(const AActor* Actor) const
{
	if(FocusedCharacter.IsValid())
	{
		FVector AimAtLocation = FocusedCharacter->GetMesh()->GetSocketLocation(DefaultAimSocket);
		if (AimAtLocation != FVector::ZeroVector)
			return AimAtLocation;
	}
    
	return Super::GetFocalPointOnActor(Actor);
}

void AAIOutlawController::SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority)
{
	Super::SetFocus(NewFocus, InPriority);
	FocusedCharacter = Cast<ABaseCharacter>(NewFocus);
}

void AAIOutlawController::GetPlayerViewPoint(FVector& Location, FRotator& Rotation) const
{
	Super::GetPlayerViewPoint(Location, Rotation);
	if (FocusedCharacter.IsValid())
	{
		FVector AimAtLocation = GetFocalPointOnActor(FocusedCharacter.Get());
		Rotation = (AimAtLocation - Location).Rotation();
	}
}
