#pragma once

#include "CoreMinimal.h"
#include "AIBaseReactionComponent.h"
#include "AI/Data/FeelingSettings.h"
#include "AI/Data/NpcReactionTypes.h"
#include "AICitizenReactionsComponent.generated.h"

USTRUCT()
struct FFeelingPropagationHistory
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Propagator = nullptr;

	float ExpiresAt = 0.f;
};

USTRUCT()
struct FRememberedTagReaction
{
	GENERATED_BODY()

	UPROPERTY()
	ABaseCharacter* Character;
	
	float RememberUntil = 0.f;
	FGameplayTag ReactionTag;
};

DECLARE_DELEGATE_TwoParams(FFeelingChangedEvent, EReactionFeelingType FeelingType, float NewValue);

class AAIController;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UAICitizenReactionsComponent : public UAIBaseReactionComponent
{
	GENERATED_BODY()

public:
	UAICitizenReactionsComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// For EQS
	TArray<FVector> GetAfraidLocations() const;
	TArray<FVector> GetInterestingLocations() const;

	// For propagation
	FPropagatedFeelingsData GetAnxietyPropagation();
	FPropagatedFeelingsData GetInterestPropagation();

	bool IsInDanger();

	void CalmDownByPlayer(const class APlayerCharacter* PlayerCharacter);
	
protected:
	virtual void BeginPlay() override;

	virtual void ReactToDamage(AActor* Actor, const FAIStimulus& Stimulus, UBlackboardComponent* Blackboard) override;
	virtual void ReactToSight(AActor* Actor, const FAIStimulus& Stimulus, UBlackboardComponent* Blackboard) override;
	void PerceiveSoundTagReaction(const FAIStimulus& Stimulus, FVector SoundLocation, float& AnxietyChange,
	                              float& InterestChange, ABaseCharacter* SoundSourceCharacter);
	virtual void ReactToSound(AActor* Actor, const FAIStimulus& Stimulus) override;

	// TODO for smart kids
	// Distance between citizen and enemy when citizen is interested (thus approaching) but not afraid of enemy 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InterestBeforeAnxietyEnemyDistance = 3000.f;

	virtual void InitializeNpc() override;
	
private:
	float Anxiety = 0.f;
	float Interest = 0.f;
	// bool bAnxious = false;
	// bool bInterested = false;
	EFeelingState AnxietyState = EFeelingState::Absent;
	EFeelingState InterestState = EFeelingState::Absent;
	
	// float Annoyance = 0.f;
	float AnxietyHoldTime = 0.f;
	float InterestHoldTime = 0.f;

	UPROPERTY()
	const UFeelingSettings* AnxietySettings;

	UPROPERTY()
	const UFeelingSettings* InterestSettings;

	UPROPERTY()
	const UFeelingPropagationSettings* FeelingPropagationSettings;

	UPROPERTY()
	TMap<EReactionSource, int> ReactionSourceEndangermentThresholds;

	TMap<EReactionSource, int> LastAnxietyReactionsIndicies;
	TMap<EReactionSource, int> LastInterestReactionsIndicies;
	TMap<EReactionSource, TArray<FReactionEvent>> AnxietyReactions; 
	TMap<EReactionSource, TArray<FReactionEvent>> InterestReactions; 

	TArray<FRememberedTagReaction> RememberedTagReactions;
	FNpcInteractWithPlayerAnxietyEffect PlayerInteractionSettings;
	float IgnoreWitnessPlayerUntil = 0.f;
	float IgnoreInteractPlayerUntil = 0.f;

	int LastPropagationHistoryIndex = 0;
	TArray<FFeelingPropagationHistory> PropagationHistory;
	
	UPROPERTY()
	TArray<AActor*> WitnessedDeadCharacters;

	void OnSeeGoodGuy(ABaseCharacter* ObservedCharacter);
	void OnSeeBadGuy(ABaseCharacter* BadGuy);
	void OnSeePlayer(ABaseCharacter* PlayerCharacter);

	void ApplyPropagatedAnxiety(const FPropagatedFeelingsData& PropagationData, float Distance);
	void ApplyPropagatedInterest(const FPropagatedFeelingsData& PropagationData, float Distance);
	void DecayFeeling(float& Feeling, float& FeelingHoldTime, float DeltaTime, float FeelDecayRate, float HoldDecayRate, const FName& BBKey);

	// TODO implement max possible feel change from an event
	float ChangeFeeling(float& Feeling, float& HoldTime, float RawDelta, const UFeelingSettings* FeelingSettings,
		const TMap<EReactionSource, TArray<FReactionEvent>>& ReactionEvents, const FVector& InstigatorLocation, const FName& BBKey,
		EReactionSource ReactionSource, EFeelingState& FeelingState, const FName& StateBBKey);

	void RememberReactionEvent(TMap<EReactionSource, TArray<FReactionEvent>>& EventMemory, EReactionSource ReactionSource,
		TMap<EReactionSource, int>& Indicies, float FeelingChange, const AActor* InstigatorActor,
		const FVector& InstigatorLocation, const TMap<EReactionSource, float>& ReactionMemoryLifetimes);

	bool IsSeeingReactionInstigator(AAIController* TestController, AActor* Actor) const;
	int GetCountOfRelevantReactions(const TArray<FReactionEvent>& ReactionsMemory) const;
	
	TArray<FVector> GetValidReactionsLocations(const TArray<FReactionEvent>& Reactions, int LastIndex) const;
	
	FPropagatedFeelingsData GetFeelingPropagation(const float& Feeling,
	const TMap<EReactionSource, TArray<FReactionEvent>>& FeelingReactions, const TMap<EReactionSource, int>& LastIndicies,
	const UFeelingSettings* FeelingSettings) const;
	
	void ApplyPropagatedFeeling(const FPropagatedFeelingsData& PropagationData, float& Feeling,
		TMap<EReactionSource, TArray<FReactionEvent>>& FeelingReactions, float& FeelingHoldTime, const UFeelingSettings* FeelingSettings,
		TMap<EReactionSource, int>& LastIndicies, EFeelingState& FeelingState, float PropagationDistanceFactor,
		const FName& FeelingLevelBBKey, const FName& StateBBKey);
	void FillFeelingPropagation(const TMap<EReactionSource, TArray<FReactionEvent>>& FeelingReactions,
		EReactionSource ReactionSource, const TMap<EReactionSource, int>& LastIndicies, TMap<EReactionSource, TArray<FReactionEvent>>& PropagatedReactions) const;

	void UpdateBlackboardFeelingState(float Feeling, const UFeelingSettings* FeelingSettings, EFeelingState& FeelingState, const FName& BBKey);
	bool CanApplyPropagationFromActor(ABaseCharacter* ObservedCharacter);

	void CalmDown(float RawAnxietyChange);
};