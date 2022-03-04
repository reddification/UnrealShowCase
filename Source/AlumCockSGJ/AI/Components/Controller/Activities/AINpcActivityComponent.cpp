#include "AINpcActivityComponent.h"

#include "AIController.h"
#include "CommonConstants.h"
#include "AI/ActivityInstances/NpcActivityInstance.h"
#include "AI/ActivityInstances/Quest/NpcQuestActivityInstance.h"
#include "AI/Controllers/AIBaseController.h"
#include "AI/Data/NpcDTR.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Characters/BaseCharacter.h"
#include "Subsystems/WorldStateSubsystem.h"

UAINpcActivityComponent::UAINpcActivityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAINpcActivityComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeNpc();
}

void UAINpcActivityComponent::InitializeActivity()
{
	Super::InitializeActivity();
	InitializeNpc();
}

UNpcActivityInstanceBase* UAINpcActivityComponent::GetActivityInstance()
{
	return RunningActivity;
}

UBlackboardComponent* UAINpcActivityComponent::GetBlackboard() const
{
	return OwnerController->GetBlackboardComponent();
}

void UAINpcActivityComponent::UpdateUtility()
{
	for (auto& WorkActivity : NpcWorkActivities)
	{
		const auto& UtilitySettings  = WorkActivity.ActivityUtilitySettings;
		if (!WorkActivity.ActivityInstance->IsActive() && WorkActivity.Utility < UtilitySettings.InitialUtility)
		{
			float UtilityChange = UtilitySettings.UtilityRestoreGain * (UtilityUpdateInterval / UtilitySettings.UtilityRestoreInterval);
			WorkActivity.Utility = FMath::Min(WorkActivity.Utility + UtilityChange, UtilitySettings.InitialUtility);
		}
	}
}

void UAINpcActivityComponent::InitializeNpc()
{
	auto ControlledCharacter = Cast<INpcCharacter>(OwnerController->GetPawn());
	if (!ControlledCharacter)
	{
		return;
		// UE_LOG(LogNpcActivity, Error, TEXT("[%s] Failed to initialize NPC behavior - no suitable character"),
		//     *GetOwner()->GetName());
	}

	// if (!ControlledCharacter->IsAiEnabled())
	// 	return;
		
	auto NpcDTRH = ControlledCharacter->GetNpcDTRH();
	if (NpcDTRH.IsNull())
		return;

	auto NpcDTR = NpcDTRH.DataTable->FindRow<FNpcDTR>(NpcDTRH.RowName, "");
	if (!NpcDTR)
		return;

	if (NpcDTR->BaseBehavior)
		OwnerController->RunBehaviorTree(NpcDTR->BaseBehavior);

	auto BTComponent = Cast<UBehaviorTreeComponent>(OwnerController->BrainComponent);
	if (!BTComponent)
		return;

	NpcRestActivities.Reset();
	for (const auto& ActivityBehavior : NpcDTR->RestBehaviors)
		SetupRestActivityBehavior(ActivityBehavior);
	
	// TODO implement time notion and switch dynamic BTs over time
	NpcWorkActivities.Reset();
	for (const auto& ActivityBehavior : NpcDTR->WorkBehaviors)
		SetupWorkActivityBehavior(ActivityBehavior);
	
	for (const auto& ReactionBehavior : NpcDTR->ReactionsBehaviors)
		SetupReactionBehavior(BTComponent, ReactionBehavior);

	if (ActivityEnergy == 1.f && NpcWorkActivities.Num() > 0)
		RunMaxUtilityActivity(BTComponent);
	else
		RunRandomRestActivity(BTComponent);

	GetWorld()->GetTimerManager().SetTimer(UtilityTimer, this, &UAINpcActivityComponent::UpdateUtility, UtilityUpdateInterval);
}

#pragma region SETUP

void UAINpcActivityComponent::SetupRestActivityBehavior(const FNpcActivityRestBehavior& BehaviorDescriptor)
{
	if (!BehaviorDescriptor.BehaviorTree)
		return;

	if (BehaviorDescriptor.ActivityInstanceClass)
	{
		auto ActivityInstance = NewObject<UNpcActivityInstanceBase>(OwnerController, BehaviorDescriptor.ActivityInstanceClass);
		ActivityInstance->NpcActivityEvent.BindUObject(this, &UAINpcActivityComponent::OnRestActivityEventOccured);
		ActivityInstance->NpcActivityStateChangedEvent.AddUObject(this, &UAINpcActivityComponent::OnRegularNpcActivityStateChanged);
		
		FNpcRestActivityWrapper NewDescriptor;

		SetupActivityWrapperBase(BehaviorDescriptor, ActivityInstance, NewDescriptor);

		NewDescriptor.ActivityEnergyRestoreSettings = BehaviorDescriptor.ActivityEnergyRestoreSettings;
		NpcRestActivities.Emplace(NewDescriptor);
	}
}

void UAINpcActivityComponent::SetupWorkActivityBehavior(const FNpcActivityWorkBehavior& BehaviorDescriptor)
{
	if (!BehaviorDescriptor.BehaviorTree)
		return;

	if (BehaviorDescriptor.ActivityInstanceClass)
	{
		auto ActivityInstance = NewObject<UNpcActivityInstanceBase>(OwnerController, BehaviorDescriptor.ActivityInstanceClass);
		ActivityInstance->NpcActivityEvent.BindUObject(this, &UAINpcActivityComponent::OnWorkActivityEventOccured);
		ActivityInstance->NpcActivityStateChangedEvent.AddUObject(this, &UAINpcActivityComponent::OnRegularNpcActivityStateChanged);
		FNpcWorkActivityWrapper NewWrapper;
		SetupActivityWrapperBase(BehaviorDescriptor, ActivityInstance, NewWrapper);

		NewWrapper.Utility = BehaviorDescriptor.ActivityUtilitySettings.InitialUtility;
		NewWrapper.ActivityUtilitySettings = BehaviorDescriptor.ActivityUtilitySettings;
		
		NpcWorkActivities.Emplace(NewWrapper);
	}
}

void UAINpcActivityComponent::SetupReactionBehavior(UBehaviorTreeComponent* BTComponent, const FNpcBehaviorReactionDescriptor& BehaviorDescriptor)
{
	if (!BehaviorDescriptor.BehaviorTree)
		return;
	
	BTComponent->SetDynamicSubtree(BehaviorDescriptor.BehaviorTag, BehaviorDescriptor.BehaviorTree);
}

void UAINpcActivityComponent::SetupActivityWrapperBase(const FNpcBehaviorActivityDescriptor& BehaviorDescriptor,
	UNpcActivityInstanceBase* ActivityInstance, FNpcBaseActivityWrapper& NewDescriptor) const
{
	NewDescriptor.ActivityInstance = ActivityInstance;
	NewDescriptor.BehaviorTree = BehaviorDescriptor.BehaviorTree;
	NewDescriptor.ActivityNecessity = BehaviorDescriptor.WorkUtility;
	ActivityInstance->Setup(BehaviorDescriptor, OwnerController);
}

#pragma endregion SETUP

#pragma region ACTIVITY EVENTS

void UAINpcActivityComponent::OnRegularNpcActivityStateChanged(const UNpcActivityInstanceBase* ActivityInstance,
	bool bActive)
{
	if (bActive)
		ResetRunningQuestActivity();
}

void UAINpcActivityComponent::OnWorkActivityEventOccured(EActivityEventType EventType)
{
	auto& RunningActivityWrapper = NpcWorkActivities[RunningWorkActivityIndex];
	if (!RunningActivityWrapper.ActivityInstance->IsActive())
		return;
	
	float EnergyChange = 0.f;
	switch (EventType)
	{
		case EActivityEventType::Assessment:
			RunningActivityWrapper.Utility -= RunningActivityWrapper.ActivityUtilitySettings.AssessmentUtilityDecay;
			EnergyChange = RunningActivityWrapper.ActivityUtilitySettings.AssessmentEnergyChange;
			break;
		case EActivityEventType::Iteration:
			RunningActivityWrapper.Utility -= RunningActivityWrapper.ActivityUtilitySettings.IterationUtilityDecay;
			EnergyChange = RunningActivityWrapper.ActivityUtilitySettings.IterationEnergyChange;
			break;
		case EActivityEventType::Completed:
			RunningActivityWrapper.Utility = 0.f;
			EnergyChange = RunningActivityWrapper.ActivityUtilitySettings.IterationEnergyChange;
			RunningActivityWrapper.CompletedTimes++;
			break;
		case EActivityEventType::ArbitraryFailure:
		case EActivityEventType::Inoperable:
		default:
			RunningActivityWrapper.Utility -= RunningActivityWrapper.ActivityUtilitySettings.ArbitraryFailureUtilityDecay;
			EnergyChange = RunningActivityWrapper.ActivityUtilitySettings.FailEnergyChange;
			break;
	}
	
	OnEnergyChanged(EnergyChange);
}

void UAINpcActivityComponent::OnRestActivityEventOccured(EActivityEventType EventType)
{
	const auto& RunningRestActivity = NpcRestActivities[RunningRestActivityIndex];
	if (!RunningRestActivity.ActivityInstance->IsActive())
		return;

	float EnergyChange = 0.f;
	switch (EventType)
	{
		case EActivityEventType::Assessment:
		case EActivityEventType::Iteration:
			EnergyChange = RunningRestActivity.ActivityEnergyRestoreSettings.EnergyRestoreGain;			
			break;
		case EActivityEventType::Inoperable:
		case EActivityEventType::ArbitraryFailure:
			Boredom = FMath::Min(1.f, Boredom + RunningRestActivity.ActivityEnergyRestoreSettings.BoredomIncreaseOnFail);
		case EActivityEventType::Completed:
			break;
	}

	OnEnergyChanged(EnergyChange);
}

void UAINpcActivityComponent::OnQuestActivityEventOccured(EActivityEventType EventType)
{
	if (RunningActivityType != ENpcActivityType::Quest)
		return;

	switch (EventType)
	{
		case EActivityEventType::Completed:
		case EActivityEventType::Timeout:
			QuestActivityFinishedEvent.ExecuteIfBound();
			RunRandomRestActivity(GetBehaviorTreeComponent());
			break;
		default:
			break;
	}
}

#pragma endregion ACTIVITY EVENTS

#pragma region ACTIVITY MANAGING

void UAINpcActivityComponent::SelectNextActivity()
{
	auto BTComponent = GetBehaviorTreeComponent();
	if (RunningActivityType != ENpcActivityType::Rest && ActivityEnergy <= 0.f)
	{
		RunRandomRestActivity(BTComponent);
	}
	else if (RunningActivityType != ENpcActivityType::Work && ActivityEnergy >= 1)
	{
		if (NpcWorkActivities.Num() > 0)
			RunMaxUtilityActivity(BTComponent);
		else if (Boredom >= 1)
			RunRandomRestActivity(BTComponent);
	}
	else if (RunningActivityType == ENpcActivityType::Work)
	{
		auto& RunningActivityWrapper = NpcWorkActivities[RunningWorkActivityIndex];
		if (RunningActivityWrapper.Utility <= 0.f)
			RunMaxUtilityActivity(BTComponent);
	}
}

void UAINpcActivityComponent::SetRunningActivityType(ENpcActivityType NewType)
{
	if (RunningActivityType != NewType)
	{
		RunningActivityType = NewType;
		OwnerController->GetBlackboardComponent()->SetValueAsEnum(BB_RunningActivityType, (uint8)NewType);
	}
}

#pragma endregion ACTIVITY MANAGING

#pragma region RUN

void UAINpcActivityComponent::RunRandomRestActivity(UBehaviorTreeComponent* BTComponent)
{
	int CountOfRestActivities = NpcRestActivities.Num();
	if (CountOfRestActivities <= 0)
	{
		SetRunningActivityType(ENpcActivityType::Work);
		return;
	}
	
	int NewRestActivityIndex = FMath::RandRange(0, CountOfRestActivities - 1);// -1;
	Boredom = 0.f;
	if (RunningActivityType == ENpcActivityType::Rest && NewRestActivityIndex == RunningRestActivityIndex)
		return;

	SetRunningActivityType(ENpcActivityType::Rest);
	
	RunningRestActivityIndex = NewRestActivityIndex;
	auto& NewRestActivity = NpcRestActivities[RunningRestActivityIndex];

	// TODO idk...
	if (RunningActivity)
		RunningActivity->Suspend(OwnerController, true);
	
	RunningActivity = NewRestActivity.ActivityInstance;
	BTComponent->GetBlackboardComponent()->SetValueAsFloat(BB_WorkUtility, NewRestActivity.ActivityNecessity);
	BTComponent->SetDynamicSubtree(RunningActivityTag, NewRestActivity.BehaviorTree);
}

void UAINpcActivityComponent::RunMaxUtilityActivity(UBehaviorTreeComponent* BTComponent)
{
	Boredom = 0.f;
	int MaxUtilityActivityIndex = RunningWorkActivityIndex >= 0 ? RunningWorkActivityIndex : 0;
	float CurrentMaxUtility = NpcWorkActivities[MaxUtilityActivityIndex].Utility;
	for (auto i = 0; i < NpcWorkActivities.Num(); i++)
	{
		float TestUtility = NpcWorkActivities[i].Utility;
		if (i != RunningWorkActivityIndex && TestUtility > CurrentMaxUtility)
		{
			MaxUtilityActivityIndex = i;
			CurrentMaxUtility = TestUtility;
		}
	}

	if (RunningActivityType == ENpcActivityType::Work && MaxUtilityActivityIndex == RunningWorkActivityIndex)
		return;

	SetRunningActivityType(ENpcActivityType::Work);
	if (RunningWorkActivityIndex >= 0)
		NpcWorkActivities[RunningWorkActivityIndex].ActivityInstance->Suspend(OwnerController, true);
	
	RunningWorkActivityIndex = MaxUtilityActivityIndex;
	auto& NewActivityDescriptor = NpcWorkActivities[RunningWorkActivityIndex];
	// NewActivityDescriptor.ActivityInstance->Resume();
	if (RunningActivity)
		RunningActivity->Suspend(OwnerController, true);

	RunningActivity = NewActivityDescriptor.ActivityInstance;
	BTComponent->GetBlackboardComponent()->SetValueAsFloat(BB_WorkUtility, NewActivityDescriptor.ActivityNecessity);
	BTComponent->SetDynamicSubtree(RunningActivityTag, NewActivityDescriptor.BehaviorTree);
}

void UAINpcActivityComponent::RunQuestActivity(const FNpcQuestBehaviorDescriptor& QuestBehaviorDescriptor)
{
	if (!QuestBehaviorDescriptor.BehaviorDescriptor.BehaviorTree)
		return;
	
	ResetRunningQuestActivity();
	
	auto ActivityInstance = NewObject<UNpcQuestActivityInstance>(OwnerController, QuestBehaviorDescriptor.BehaviorDescriptor.ActivityInstanceClass);
	ActivityInstance->NpcActivityEvent.BindUObject(this, &UAINpcActivityComponent::OnQuestActivityEventOccured);
	auto BTComponent = GetBehaviorTreeComponent();
	
	SetupActivityWrapperBase(QuestBehaviorDescriptor.BehaviorDescriptor, ActivityInstance, RunningQuestActivity);
	ActivityInstance->SetStopConditions(OwnerController, QuestBehaviorDescriptor);
	
	BTComponent->GetBlackboardComponent()->SetValueAsFloat(BB_WorkUtility, QuestBehaviorDescriptor.BehaviorDescriptor.WorkUtility);

	// TODO perhaps check if run until world state haven't occured already
	if (IsValid(RunningActivity))
		RunningActivity->Suspend(OwnerController, IsInteractionInterruptionRequired());
		
	RunningActivity = ActivityInstance;
	BTComponent->SetDynamicSubtree(RunningActivityTag, QuestBehaviorDescriptor.BehaviorDescriptor.BehaviorTree);
	SetRunningActivityType(ENpcActivityType::Quest);
}

#pragma endregion RUN

#pragma region INTERACTION

const FNpcActorInteractionData* UAINpcActivityComponent::GetCurrentNpcInteractionData() const
{
	return &CurrentInteractionData;
}

void UAINpcActivityComponent::SetCurrentNpcInteractionData(AActor* Actor, ENpcActorInteractionState State,
	uint32 ActivityId)
{
	CurrentInteractionData.SetState(Actor, State);
	CurrentInteractionData.ActivityId = ActivityId;
}

void UAINpcActivityComponent::SetCurrentNpcInteractionDataState(uint32 ActivityId, ENpcActorInteractionState State)
{
	if (!CurrentInteractionData.IsNone() && CurrentInteractionData.ActivityId == ActivityId)
		CurrentInteractionData.State = State;
}

void UAINpcActivityComponent::ResetActorInteractionData()
{
	CurrentInteractionData.Reset();
}

bool UAINpcActivityComponent::IsInteractionInterruptionRequired() const
{
	if ((bInDialogue || bInConversation) && CurrentInteractionData.IsInteracting())
	{
		auto Npc = Cast<ABaseCharacter>(OwnerController->GetCharacter());
		FGameplayTag InteractionTag;
		if (CurrentInteractionData.InteractableActor->GetNpcActiveInteractionTag(Npc, InteractionTag))
			return InteruptableByDialogue.HasTagExact(InteractionTag);
	}
	
	return true;
}

void UAINpcActivityComponent::ProlongInteractionMemory(float ProlongationTime)
{
	CurrentInteractionData.RememberUntilWorldTime = GetWorld()->GetTimeSeconds() + ProlongationTime;
}

#pragma endregion INTERACTION

#pragma region DIALOGUES

void UAINpcActivityComponent::StartDialogueMode(bool bFollowPlayer)
{
	bInDialogue = true;
	GetBlackboard()->SetValueAsBool("bFollowPlayer", bFollowPlayer);
	GetBlackboard()->SetValueAsBool("bInDialogue", true);
}

void UAINpcActivityComponent::EndDialogueMode()
{
	bInDialogue = false;
	GetBlackboard()->ClearValue("bFollowPlayer");
	GetBlackboard()->SetValueAsBool("bInDialogue", false);
}

#pragma endregion DIALOGUES

void UAINpcActivityComponent::ResetRunningQuestActivity()
{
	if (IsValid(RunningQuestActivity.ActivityInstance))
	{
		// RunningQuestActivity.ActivityInstance->Suspend(OwnerController, true);
		if (IsValid(RunningQuestActivity.ActivityInstance))
			RunningQuestActivity.ActivityInstance->ConditionalBeginDestroy();
		
		RunningQuestActivity = FNpcQuestActivityWrapper();
	}
}

void UAINpcActivityComponent::OnEnergyChanged(float EnergyChange)
{
	ActivityEnergy = FMath::Clamp(ActivityEnergy + EnergyChange, 0.f, 1.f);
	SelectNextActivity();
}

void UAINpcActivityComponent::Rest(float DeltaTime)
{
	if (RunningActivityType != ENpcActivityType::Rest)
		return;

	auto& RestoreSettings = NpcRestActivities[RunningRestActivityIndex].ActivityEnergyRestoreSettings;
	Boredom = FMath::Min(Boredom + RestoreSettings.BoredomTimedIncrease * DeltaTime / RestoreSettings.BoredomAccumulationInterval, 1.f);
	OnEnergyChanged(RestoreSettings.EnergyRestoreGain * DeltaTime / RestoreSettings.EnergyRestoreInterval);
}

UBehaviorTreeComponent* UAINpcActivityComponent::GetBehaviorTreeComponent() const
{
	return StaticCast<UBehaviorTreeComponent*>(OwnerController->GetBrainComponent());
}