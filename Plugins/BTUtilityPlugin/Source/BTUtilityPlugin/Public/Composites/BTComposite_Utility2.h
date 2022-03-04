#pragma once

#include "BehaviorTree/BTCompositeNode.h"
#include "Composites/BTComposite_Utility.h"
#include "BTComposite_Utility2.generated.h"

UCLASS()
class BTUTILITYPLUGIN_API UBTComposite_Utility2: public UBTCompositeNode
{
	GENERATED_UCLASS_BODY()

public:
	// Method used to determine child selection based on utility values
	UPROPERTY(VisibleAnywhere, Category = "Utility")
	EUtilitySelectionMethod SelectionMethod = EUtilitySelectionMethod::Priority;

	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	virtual void CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual FString GetStaticDescription() const override;
	
protected:
	bool EvaluateUtilityScores(FBehaviorTreeSearchData& SearchData, TArray<float>& OutScores) const;
	const UBTDecorator_UtilityFunction* FindChildUtilityFunction(int32 ChildIndex) const;
	
	/**
	 * Gets all the scores via EvaluateUtilityScores, then sorts them for selection.
	 */
	virtual void OrderUtilityScores(FBehaviorTreeSearchData& SearchData) const;

	/**
	 * Called when start enters this node.
	 * 
	 * Evaluate utilities,
	 * and set up child BTDecorator_UtilityBlackboards to monitor their blackboard keys so that they can abort and we can reevaluate if they change.
	 */
	virtual void NotifyNodeActivation(FBehaviorTreeSearchData& SearchData) const override;
	
	/**
	 * Called when start leaves this node.
	 * 
	 * Stop monitoring for blackboard key changes.
	 */
	virtual void NotifyNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult) const override;

	/**
	 * Set up child BTDecorator_UtilityBlackboard to monitor their blackboard keys so that they can abort and reevaluate if they change.
	 * We have to do this here because we're always active when our children are executing, but non-executing children aren't active when other children are,
	 * so they can't register and unregister in OnBecomeRelevant and OnCeaseRelevant because they're always CeaseRelevant unless they're executing.
	 * Additionally we can't register as the observer (i.e. do the whole thing in this node) as all composite (us) node functions are const, so we can't pass `this`
	 * to the observer function.
	 * 
	 * So: when we're entered, we iterate through our children and  tell the blackboard to alert our child blackboard utility decorators when their value changes.
	 * Then when a value changes the matching decorator requests execution, which triggers our NotifyNodeActivation(), which reevaluates all utilities.
	 */
	virtual void WatchChildBlackboardKeys(FBehaviorTreeSearchData& SearchData) const;

	/**
	 * Stop monitoring the blackboard keys of all our BTDecorator_UtilityBlackboard children.
	 */
	virtual void UnwatchChildBlackboardKeys(FBehaviorTreeSearchData& SearchData) const;

public:
	virtual int32 GetNextChildHandler(struct FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const override;
	
#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif
};

