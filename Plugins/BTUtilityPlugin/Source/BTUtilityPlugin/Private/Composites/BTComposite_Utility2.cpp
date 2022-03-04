#include "Composites/BTComposite_Utility2.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Decorators/BTDecorator_UtilityBlackboard.h"
#include "UtilitySelectionMethods/BTUtilitySelectionMethod_Highest.h"
#include "UtilitySelectionMethods/BTUtilitySelectionMethod_Proportional.h"

UBTComposite_Utility2::UBTComposite_Utility2(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Utility 2.0";
	bUseNodeActivationNotify = true;
	bUseNodeDeactivationNotify = true;

	SelectionMethod = EUtilitySelectionMethod::Priority;

	// OnNextChild.BindUObject(this, &UBTComposite_Utility2::GetNextChildHandler);
}

void UBTComposite_Utility2::NotifyNodeActivation(FBehaviorTreeSearchData& SearchData) const
{
	auto NodeMemory = GetNodeMemory<FBTUtilityMemory>(SearchData);
	if (NodeMemory->UtilityEvaluation == EUtilityEvaluation::Blackboard)
	{
		// Reseting until next time
		NodeMemory->UtilityEvaluation = EUtilityEvaluation::None;
	}
	else
	{
		OrderUtilityScores(SearchData);

		// monitor for changes to utility while we're executing
		WatchChildBlackboardKeys(SearchData);	
	}
}

void UBTComposite_Utility2::NotifyNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult) const
{
	// stop monitoring for changes to utility now that we're no longer executing
	UnwatchChildBlackboardKeys(SearchData);
}

bool UBTComposite_Utility2::EvaluateUtilityScores(FBehaviorTreeSearchData& SearchData, TArray<float>& OutScores) const
{
	bool bIsNonZeroScore = false;
	// Loop through utility children
	for(int32 Idx = 0; Idx < GetChildrenNum(); ++Idx)
	{
		auto UtilityFunc = FindChildUtilityFunction(Idx);
		
		// Calculate utility value
		auto Score = UtilityFunc ?
			UtilityFunc->WrappedCalculateUtility(SearchData.OwnerComp, UtilityFunc->GetNodeMemory<uint8>(SearchData)) :
			0.0f;

		OutScores.Add(Score);
		bIsNonZeroScore = bIsNonZeroScore || Score > 0.0f;
	}

	return bIsNonZeroScore;
}

const UBTDecorator_UtilityFunction* UBTComposite_Utility2::FindChildUtilityFunction(int32 ChildIndex) const
{
	auto const& ChildInfo = Children[ChildIndex];
	for (auto Dec : ChildInfo.Decorators)
	{
		auto AsUtilFunc = Cast<UBTDecorator_UtilityFunction>(Dec);
		if (AsUtilFunc)
		{
			// Take the first one. Multiple utility function decorators on a single node is a user
			// error, and generates a warning in the behavior tree editor.
			return AsUtilFunc;
		}
	}

	// Child does not have a utility function decorator
	return nullptr;
}

void UBTComposite_Utility2::OrderUtilityScores(FBehaviorTreeSearchData& SearchData) const
{
	FBTUtilityMemory* NodeMemory = GetNodeMemory<FBTUtilityMemory>(SearchData);

	// Evaluate utility scores for each child
	TArray<float> UtilityValues;
	EvaluateUtilityScores(SearchData, UtilityValues);
	// Generate ordering
	switch (SelectionMethod)
	{
		case EUtilitySelectionMethod::Priority:
			UtilitySelection::PriorityOrdering2(UtilityValues, NodeMemory);
			break;
		case EUtilitySelectionMethod::Proportional:
			// TODO unsupported
			// UtilitySelection::ProportionalOrdering(UtilityValues, NodeMemory->ExecutionOrdering);
			break;
		default:
			check(false);
			break;
	}
}

void UBTComposite_Utility2::WatchChildBlackboardKeys(FBehaviorTreeSearchData& SearchData) const
{
	// for each child task
	for (int32 Idx = 0; Idx < GetChildrenNum(); ++Idx)
	{
		// for each of their decorators
		FBTCompositeChild const& ChildInfo = Children[Idx];
		for (UBTDecorator* Dec : ChildInfo.Decorators)
		{
			// look for a UtilityBlackboard one
			UBTDecorator_UtilityBlackboard* AsBlackboardUtility = Cast<UBTDecorator_UtilityBlackboard>(Dec);
			if (AsBlackboardUtility)
			{
				// get the key
				FBlackboardKeySelector SelectedBlackboardKeySelector = AsBlackboardUtility->GetSelectedBlackboardKeySelector();

				// watch its key
				UBlackboardComponent* BlackboardComp = SearchData.OwnerComp.GetBlackboardComponent();
				if (BlackboardComp)
				{
					auto KeyID = SelectedBlackboardKeySelector.GetSelectedKeyID();
					BlackboardComp->RegisterObserver(KeyID, AsBlackboardUtility,
						FOnBlackboardChangeNotification::CreateUObject(AsBlackboardUtility, &UBTDecorator_UtilityBlackboard::OnBlackboardKeyValueChange));
				}

				// Just use the first one. Multiple utility function decorators on a single node is a user
				// error, and generates a warning in the behavior tree editor.
				break;
			}
		}
	}
}

void UBTComposite_Utility2::UnwatchChildBlackboardKeys(FBehaviorTreeSearchData& SearchData) const
{
	// for each child task
	for (int32 Idx = 0; Idx < GetChildrenNum(); ++Idx)
	{
		// for each of their decorators
		FBTCompositeChild const& ChildInfo = Children[Idx];
		for (UBTDecorator* Dec : ChildInfo.Decorators)
		{
			// look for a UtilityBlackboard one
			UBTDecorator_UtilityBlackboard* AsBlackboardUtility = Cast<UBTDecorator_UtilityBlackboard>(Dec);
			if (AsBlackboardUtility)
			{
				// get the key
				FBlackboardKeySelector SelectedBlackboardKeySelector = AsBlackboardUtility->GetSelectedBlackboardKeySelector();

				// unwatch its key
				UBlackboardComponent* BlackboardComp = SearchData.OwnerComp.GetBlackboardComponent();
				if (BlackboardComp)
				{
					BlackboardComp->UnregisterObserversFrom(AsBlackboardUtility);
				}

				// Just use the first one. Multiple utility function decorators on a single node is a user
				// error, and generates a warning in the behavior tree editor.
				break;
			}
		}
	}
}

int32 UBTComposite_Utility2::GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const
{
	FBTUtilityMemory* NodeMemory = GetNodeMemory<FBTUtilityMemory>(SearchData);

	// success = quit
	int32 NextChildIdx = BTSpecialChild::ReturnToParent;

	// check(NodeMemory->ExecutionOrdering.Num() == GetChildrenNum());
	if (PrevChild == BTSpecialChild::NotInitialized)
	{
		// newly activated: start from first in the ordering
		// NextChildIdx = NodeMemory->ExecutionOrdering[0];
		NextChildIdx = NodeMemory->ExecutionUtilityOrdering[0].ChildIdx;
	}
	else if (LastResult == EBTNodeResult::Failed || NodeMemory->UtilityEvaluation == EUtilityEvaluation::Blackboard)
	{
		NodeMemory->UtilityEvaluation = EUtilityEvaluation::None;
		if (NodeMemory->ExecutionUtilityOrdering[0].UtilityScore > 0)
			NextChildIdx = NodeMemory->ExecutionUtilityOrdering[0].ChildIdx;
	}

	if (NextChildIdx == PrevChild)
	{
		// a bit hacky but still...
		NextChildIdx = BTSpecialChild::ReturnToParent;	
	}
	
	return NextChildIdx;
}

uint16 UBTComposite_Utility2::GetInstanceMemorySize() const
{
	return sizeof(FBTUtilityMemory);
}

#if WITH_EDITOR

FName UBTComposite_Utility2::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Composite.Utility.Icon");
}

#endif

void UBTComposite_Utility2::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
	FBTUtilityMemory* UtilityMemory = reinterpret_cast<FBTUtilityMemory*>(NodeMemory);
	if (InitType == EBTMemoryInit::Initialize)
	{
		// Invoke the constructor for our TArray
		new (&UtilityMemory->ExecutionOrdering) FUtilityExecutionOrdering();
	}
}

void UBTComposite_Utility2::CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTMemoryClear::Type CleanupType) const
{
	Super::CleanupMemory(OwnerComp, NodeMemory, CleanupType);
	FBTUtilityMemory* UtilityMemory = reinterpret_cast<FBTUtilityMemory*>(NodeMemory);
	UtilityMemory->UtilityScores.Reset();
	UtilityMemory->ExecutionOrdering.Reset();
	UtilityMemory->ExecutionUtilityOrdering.Reset();
}

FString UBTComposite_Utility2::GetStaticDescription() const
{
	switch (SelectionMethod)
	{
	case EUtilitySelectionMethod::Priority:
		return TEXT("Priority selection");

	case EUtilitySelectionMethod::Proportional:
		return TEXT("Proportional selection");

	default:
		check(false);
		return{};
	}
}
