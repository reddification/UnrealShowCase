#include "AIDogController2.h"

#include "AI/Components/Controller/Activities/AINpcActivityComponent.h"
#include "AI/Components/Controller/Reactions/AICitizenReactionsComponent.h"
#include "Perception/AIPerceptionComponent.h"

AAIDogController2::AAIDogController2(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer
    	.SetDefaultSubobjectClass<UAINpcActivityComponent>(AAIBaseController::ActivitiesComponentName)
    	.SetDefaultSubobjectClass<UPathFollowingComponent>(TEXT("PathFollowingComponent")))
{
	NpcActivitiesComponent = Cast<UAINpcActivityComponent>(ActivitiesComponent);
	PrimaryActorTick.bCanEverTick = 1;
}

void AAIDogController2::BeginPlay()
{
	Super::BeginPlay();
	auto NpcCharacter = Cast<INpcCharacter>(GetPawn());
	
	if (NpcCharacter)
	{
		if (!NpcCharacter->IsAiEnabled())
		{
			SetAIEnabled(false);
			// UAIPerceptionSystem* AIPerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld());
			// AIPerceptionSys->UnregisterSource(*GetPawn());
			// AIPerceptionSys->UnregisterSource(*this);
		}
		else InitializeNpc(NpcCharacter);
	}
}

void AAIDogController2::SetAIEnabled(bool bNewState)
{
	if (BrainComponent)
	{
		if (bNewState)
		{
			if (!BrainComponent->IsRunning())
				BrainComponent->RestartLogic();
		}
		else
		{
			BrainComponent->StopLogic("Manual");
		}
	}
	
	if (PerceptionComponent)
	{
		PerceptionComponent->SetActive(bNewState);
		PerceptionComponent->SetComponentTickEnabled(bNewState);
	}
	
	ActivitiesComponent->SetActive(bNewState);
	ActivitiesComponent->SetComponentTickEnabled(bNewState);
	ReactionsComponent->SetActive(bNewState);
	ReactionsComponent->SetComponentTickEnabled(bNewState);
}

void AAIDogController2::SetNavigationFilterClass(const TSubclassOf<UNavigationQueryFilter>& CustomNavigationFilter)
{
	InitialNavigationFilterClass = DefaultNavigationFilterClass;
	DefaultNavigationFilterClass = CustomNavigationFilter;
}

void AAIDogController2::ResetNavigationFilterClass()
{
	DefaultNavigationFilterClass = InitialNavigationFilterClass;
}

void AAIDogController2::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	auto NpcCharacter = Cast<INpcCharacter>(InPawn);
	if (NpcCharacter)
		InitializeNpc(NpcCharacter);
}

UBlackboardComponent* AAIDogController2::GetBlackboard() const
{
	return Blackboard;
}

ABaseCharacter* AAIDogController2::GetActivityCharacter() const
{
	return ControlledCharacter;
}

void AAIDogController2::Restore(float DeltaSeconds)
{
	NpcActivitiesComponent->Rest(DeltaSeconds);
}

UNpcActivityInstanceBase* AAIDogController2::GetRunningActivity()
{
	return NpcActivitiesComponent->GetActivityInstance();
}

void AAIDogController2::RunQuestActivity(const FNpcQuestBehaviorDescriptor& BehaviorDescriptor)
{
	NpcActivitiesComponent->RunQuestActivity(BehaviorDescriptor);
}

ENpcActivityType AAIDogController2::GetRunningActivityType() const
{
	return NpcActivitiesComponent->GetRunningActivityType();
}

const FNpcActorInteractionData* AAIDogController2::GetCurrentNpcInteractionData() const
{
	return NpcActivitiesComponent->GetCurrentNpcInteractionData();
}

void AAIDogController2::SetCurrentNpcInteractionData(AActor* Actor, ENpcActorInteractionState State,
	uint32 ActivityId)
{
	NpcActivitiesComponent->SetCurrentNpcInteractionData(Actor, State, ActivityId);
}

void AAIDogController2::SetCurrentNpcInteractionDataState(uint32 ActivityId, ENpcActorInteractionState State)
{
	NpcActivitiesComponent->SetCurrentNpcInteractionDataState(ActivityId, State);
}

void AAIDogController2::ResetActorInteractionData()
{
	NpcActivitiesComponent->ResetActorInteractionData();
}

void AAIDogController2::ProlongInteractionMemory(float ProlongationTime)
{
	NpcActivitiesComponent->ProlongInteractionMemory(ProlongationTime);	
}

void AAIDogController2::OnDialogueStarted(bool bFollowPlayer)
{
	NpcActivitiesComponent->StartDialogueMode(bFollowPlayer);
}

void AAIDogController2::OnDialogueEnded()
{
	NpcActivitiesComponent->EndDialogueMode();
}

void AAIDogController2::TryAbortDialogue() const
{
	NpcActivitiesComponent->EndDialogueMode();
}

bool AAIDogController2::IsInteractionInterruptionRequired() const
{
	return NpcActivitiesComponent->IsInteractionInterruptionRequired();
}

void AAIDogController2::InitializeNpc(INpcCharacter* NpcCharacter)
{
	const auto& NpcDTRH = NpcCharacter->GetNpcDTRH();
	if (NpcDTRH.IsNull())
		return;

	FNpcDTR* NpcDTR = NpcDTRH.GetRow<FNpcDTR>("");
	if (!NpcDTR)
		return;
	
	if (NpcDTR->DefaultNavigationQueryFilterClass)
		SetDefaultNavigationQueryFilterClass(NpcDTR->DefaultNavigationQueryFilterClass);
}
