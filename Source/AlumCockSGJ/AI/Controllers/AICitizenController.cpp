#include "AICitizenController.h"
#include "AI/Characters/AICitizenCharacter.h"
#include "AI/Components/Controller/Activities/AINpcActivityComponent.h"
#include "AI/Components/Controller/Reactions/AICitizenReactionsComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"

AAICitizenController::AAICitizenController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer
    	.SetDefaultSubobjectClass<UAINpcActivityComponent>(AAIBaseController::ActivitiesComponentName)
    	.SetDefaultSubobjectClass<UAICitizenReactionsComponent>(AAIBaseController::ReactionsComponentName)
    	.SetDefaultSubobjectClass<UPathFollowingComponent>(TEXT("PathFollowingComponent")))
{
	NpcActivitiesComponent = Cast<UAINpcActivityComponent>(ActivitiesComponent);
	CitizenReactionsComponent = Cast<UAICitizenReactionsComponent>(ReactionsComponent);
	PrimaryActorTick.bCanEverTick = 1;
}

void AAICitizenController::BeginPlay()
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

	NpcActivitiesComponent->QuestActivityFinishedEvent.BindUObject(this, &AAICitizenController::OnQuestActivityFinished);
}

void AAICitizenController::SetAIEnabled(bool bNewState)
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

void AAICitizenController::SetNavigationFilterClass(const TSubclassOf<UNavigationQueryFilter>& CustomNavigationFilter)
{
	InitialNavigationFilterClass = DefaultNavigationFilterClass;
	DefaultNavigationFilterClass = CustomNavigationFilter;
}

void AAICitizenController::ResetNavigationFilterClass()
{
	DefaultNavigationFilterClass = InitialNavigationFilterClass;
}

void AAICitizenController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	auto NpcCharacter = Cast<INpcCharacter>(InPawn);
	if (NpcCharacter)
		InitializeNpc(NpcCharacter);
}

UBlackboardComponent* AAICitizenController::GetBlackboard() const
{
	return Blackboard;
}

ABaseCharacter* AAICitizenController::GetActivityCharacter() const
{
	return ControlledCharacter;
}

TArray<FVector> AAICitizenController::GetAfraidLocations()
{
	return CitizenReactionsComponent->GetAfraidLocations();
}

TArray<FVector> AAICitizenController::GetInterestingLocations()
{
	return CitizenReactionsComponent->GetInterestingLocations();
}

FPropagatedFeelingsData AAICitizenController::GetAnxietyPropagation()
{
	return CitizenReactionsComponent->GetAnxietyPropagation();
}

FPropagatedFeelingsData AAICitizenController::GetInterestPropagation()
{
	return CitizenReactionsComponent->GetInterestPropagation();
}

void AAICitizenController::Restore(float DeltaSeconds)
{
	NpcActivitiesComponent->Rest(DeltaSeconds);
}

UNpcActivityInstanceBase* AAICitizenController::GetRunningActivity()
{
	return NpcActivitiesComponent->GetActivityInstance();
}

void AAICitizenController::RunQuestActivity(const FNpcQuestBehaviorDescriptor& BehaviorDescriptor)
{
	NpcActivitiesComponent->RunQuestActivity(BehaviorDescriptor);
	RunningQuestActivityParameters.bBlockSenses = BehaviorDescriptor.bBlockFeelings;
	if (RunningQuestActivityParameters.bBlockSenses)
		CitizenReactionsComponent->SetSensesBlocked(true);
}

ENpcActivityType AAICitizenController::GetRunningActivityType() const
{
	return NpcActivitiesComponent->GetRunningActivityType();
}

bool AAICitizenController::IsInDanger()
{
	return CitizenReactionsComponent->IsInDanger();
}

const FNpcActorInteractionData* AAICitizenController::GetCurrentNpcInteractionData() const
{
	return NpcActivitiesComponent->GetCurrentNpcInteractionData();
}

void AAICitizenController::SetCurrentNpcInteractionData(AActor* Actor, ENpcActorInteractionState State,
	uint32 ActivityId)
{
	NpcActivitiesComponent->SetCurrentNpcInteractionData(Actor, State, ActivityId);
}

void AAICitizenController::SetCurrentNpcInteractionDataState(uint32 ActivityId, ENpcActorInteractionState State)
{
	NpcActivitiesComponent->SetCurrentNpcInteractionDataState(ActivityId, State);
}

void AAICitizenController::ResetActorInteractionData()
{
	NpcActivitiesComponent->ResetActorInteractionData();
}

void AAICitizenController::ProlongInteractionMemory(float ProlongationTime)
{
	NpcActivitiesComponent->ProlongInteractionMemory(ProlongationTime);	
}

void AAICitizenController::OnDialogueStarted(bool bFollowPlayer)
{
	NpcActivitiesComponent->StartDialogueMode(bFollowPlayer);
}

void AAICitizenController::OnDialogueEnded()
{
	NpcActivitiesComponent->EndDialogueMode();
}

void AAICitizenController::TryAbortDialogue() const
{
	NpcActivitiesComponent->EndDialogueMode();
}

bool AAICitizenController::IsInteractionInterruptionRequired() const
{
	return NpcActivitiesComponent->IsInteractionInterruptionRequired();
}

void AAICitizenController::OnQuestActivityFinished()
{
	if (RunningQuestActivityParameters.bBlockSenses)
		CitizenReactionsComponent->SetSensesBlocked(false);
}

void AAICitizenController::InitializeNpc(INpcCharacter* NpcCharacter)
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
