#include "BTService_EnemyBehavior.h"

#include "AIController.h"
#include "AI/Data/AITypesGC.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BaseCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"

UBTService_EnemyBehavior::UBTService_EnemyBehavior()
{
	NodeName = "Enemy behavior";
	CurrentTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_EnemyBehavior, CurrentTargetKey), APawn::StaticClass());
	InterestingLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_EnemyBehavior, InterestingLocationKey));
	CombatModeKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_EnemyBehavior, CombatModeKey), StaticEnum<EAICombatReason>());
}

void UBTService_EnemyBehavior::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	auto PerceptionComponent = OwnerComp.GetAIOwner()->GetPerceptionComponent();
	auto BlackboardComponent = OwnerComp.GetBlackboardComponent();
	auto Pawn = OwnerComp.GetAIOwner()->GetPawn();
	FBTSeviceMemory* ServiceMemory = (FBTSeviceMemory*)NodeMemory;
	
	auto CurrentTarget = BlackboardComponent->GetValueAsObject(CurrentTargetKey.SelectedKeyName);
	auto CurrentCombatMode = (EAICombatReason)BlackboardComponent->GetValueAsEnum(CombatModeKey.SelectedKeyName);

	auto AIController = OwnerComp.GetAIOwner();
	if (IsSensingDamage(AIController, PerceptionComponent, BlackboardComponent, ServiceMemory, CurrentTarget, CurrentCombatMode))
		return;

	if (IsSensingVisual(AIController, PerceptionComponent, BlackboardComponent, ServiceMemory, CurrentTarget, CurrentCombatMode))
		return;

	if (IsSensingSound(AIController, PerceptionComponent, BlackboardComponent, ServiceMemory, CurrentTarget, CurrentCombatMode))
		return;
	
	// if not sensing anything
	if (CurrentTarget)
	{
		BlackboardComponent->ClearValue(CurrentTargetKey.SelectedKeyName);
		if (CurrentCombatMode != EAICombatReason::None)
			BlackboardComponent->ClearValue(CombatModeKey.SelectedKeyName);
	}

	AIController->ClearFocus(EAIFocusPriority::Gameplay);
	if (ServiceMemory->InterestingLocation != FVector::ZeroVector)
		BlackboardComponent->SetValueAsVector(InterestingLocationKey.SelectedKeyName, ServiceMemory->InterestingLocation);
}

bool UBTService_EnemyBehavior::IsSensingDamage(AAIController* AIController, UAIPerceptionComponent* PerceptionComponent, UBlackboardComponent* BlackboardComponent,
	UBTService_EnemyBehavior::FBTSeviceMemory* ServiceMemory, UObject* CurrentTarget, EAICombatReason CurrentCombatMode)
{
	ABaseCharacter* ControlledCharacter = Cast<ABaseCharacter>(AIController->GetPawn());
	ABaseCharacter* TargetCharacter = nullptr;
	float MaxReceivedDamage = 0.f;
	const FAISenseID SenseID = UAISense::GetSenseID(UAISense_Damage::StaticClass());
	for (auto DataIt = PerceptionComponent->GetPerceptualDataConstIterator(); DataIt; ++DataIt)
	{
		const bool bWasEverPerceived = DataIt->Value.HasKnownStimulusOfSense(SenseID);
		if (bWasEverPerceived)
		{
			if (DataIt->Value.Target.IsValid())
			{
				ABaseCharacter* DamagingCharacter = Cast<ABaseCharacter>(DataIt->Value.Target);
				if (!DamagingCharacter || !DamagingCharacter->IsAlive() || IgnoreTarget(DamagingCharacter)
					|| (!bIgnoreTeamDamage && ControlledCharacter->GetTeam() == DamagingCharacter->GetTeam()))
				{
					continue;
				}
				
				float CurrentDamage = DataIt->Value.LastSensedStimuli[SenseID].Strength;
				if (CurrentDamage > MaxReceivedDamage)
				{
					MaxReceivedDamage = CurrentDamage;
					TargetCharacter = DamagingCharacter;
				}
			}
		}
	}

	if (!TargetCharacter)
		return false;
	
	ServiceMemory->InterestingLocation = TargetCharacter->GetActorLocation();
	
	if (CurrentTarget != TargetCharacter)
	{
		AIController->SetFocus(TargetCharacter);
		BlackboardComponent->SetValueAsObject(CurrentTargetKey.SelectedKeyName, TargetCharacter);
		if (CurrentCombatMode != EAICombatReason::Defend)
			BlackboardComponent->SetValueAsEnum(CombatModeKey.SelectedKeyName, (uint8)EAICombatReason::Defend);

		BlackboardComponent->ClearValue(InterestingLocationKey.SelectedKeyName);
	}
	
	return true;
}

bool UBTService_EnemyBehavior::IsSensingVisual(AAIController* AIController, UAIPerceptionComponent* PerceptionComponent, UBlackboardComponent* BlackboardComponent,
	UBTService_EnemyBehavior::FBTSeviceMemory* ServiceMemory, UObject* CurrentTarget, EAICombatReason CurrentCombatMode)
{
	ABaseCharacter* TargetCharacter = nullptr;
	auto ControlledPawn = AIController->GetPawn();
	auto ControlledCharacter = Cast<ABaseCharacter>(ControlledPawn);
	float ClosestTargetSquareDistance = FLT_MAX;
	const FAISenseID SenseID = UAISense::GetSenseID(UAISense_Sight::StaticClass());
	for (auto DataIt = PerceptionComponent->GetPerceptualDataConstIterator(); DataIt; ++DataIt)
	{
		const bool bWasEverPerceived = DataIt->Value.HasKnownStimulusOfSense(SenseID);
		if (bWasEverPerceived)
		{
			if (DataIt->Value.Target.IsValid())
			{
				ABaseCharacter* VisualTarget = Cast<ABaseCharacter>(DataIt->Value.Target);
				if (!VisualTarget || !VisualTarget->IsAlive() || IgnoreTarget(VisualTarget) || VisualTarget->GetTeam() == ControlledCharacter->GetTeam())
					continue;

				float SqDistance = FVector::DistSquared(ControlledPawn->GetActorLocation(), VisualTarget->GetActorLocation());
				if (SqDistance < ClosestTargetSquareDistance)
				{
					ClosestTargetSquareDistance = SqDistance;
					TargetCharacter = VisualTarget;
				}
			}
		}
	}

	if (!TargetCharacter)
		return false;
	
	ServiceMemory->InterestingLocation = TargetCharacter->GetActorLocation();
	
	if (CurrentTarget != TargetCharacter)
	{
		AIController->SetFocus(TargetCharacter);
		BlackboardComponent->SetValueAsObject(CurrentTargetKey.SelectedKeyName, TargetCharacter);
		if (CurrentCombatMode != EAICombatReason::Visual)
			BlackboardComponent->SetValueAsEnum(CombatModeKey.SelectedKeyName, (uint8)EAICombatReason::Visual);

		BlackboardComponent->ClearValue(InterestingLocationKey.SelectedKeyName);
	}
	
	return true;
}

bool UBTService_EnemyBehavior::IsSensingSound(AAIController* AIController, UAIPerceptionComponent* PerceptionComponent, UBlackboardComponent* BlackboardComponent,
	UBTService_EnemyBehavior::FBTSeviceMemory* ServiceMemory, UObject* CurrentTarget, EAICombatReason CurrentCombatMode)
{
	FVector InterestingLocation = FVector::ZeroVector;
	auto ControlledPawn = AIController->GetPawn();
	float ClosestTargetSquareDistance = FLT_MAX;
	const FAISenseID SenseID = UAISense::GetSenseID(UAISense_Hearing::StaticClass());
	for (auto DataIt = PerceptionComponent->GetPerceptualDataConstIterator(); DataIt; ++DataIt)
	{
		const bool bWasEverPerceived = DataIt->Value.HasKnownStimulusOfSense(SenseID);
		if (bWasEverPerceived)
		{
			if (DataIt->Value.Target.IsValid())
			{
				float SqDistance = FVector::DistSquared(ControlledPawn->GetActorLocation(), DataIt->Value.Target->GetActorLocation());
				if (SqDistance < ClosestTargetSquareDistance)
				{
					ClosestTargetSquareDistance = SqDistance;
					InterestingLocation = DataIt->Value.Target->GetActorLocation();
				}
			}
		}
	}

	if (InterestingLocation == FVector::ZeroVector)
		return false;
	
	ServiceMemory->InterestingLocation = InterestingLocation;

	BlackboardComponent->ClearValue(CurrentTargetKey.SelectedKeyName);
	BlackboardComponent->ClearValue(CombatModeKey.SelectedKeyName);
	BlackboardComponent->SetValueAsVector(InterestingLocationKey.SelectedKeyName, InterestingLocation);
	return true;
}

bool UBTService_EnemyBehavior::IgnoreTarget(ABaseCharacter* Target)
{
	if (IgnoreActorsWithTag.IsEmpty())
		return false;

	FGameplayTagContainer CharacterGameplayTags;
	Target->GetOwnedGameplayTags(CharacterGameplayTags);
	return IgnoreActorsWithTag.Matches(CharacterGameplayTags);
}

FString UBTService_EnemyBehavior::GetStaticDescription() const
{
	FString ServiceDescription = FString::Printf(TEXT("\nCurrent target: %s\nInteresting location: %s\nCombat mode: %s"),
		*CurrentTargetKey.SelectedKeyName.ToString(), *InterestingLocationKey.SelectedKeyName.ToString(), *CombatModeKey.SelectedKeyName.ToString());
	return Super::GetStaticDescription().Append(ServiceDescription);
}
