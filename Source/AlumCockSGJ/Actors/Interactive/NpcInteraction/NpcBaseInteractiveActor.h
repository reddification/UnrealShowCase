#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "AI/Data/DelegatesAI.h"
#include "AI/Interfaces/NpcInteractableActor.h"
#include "Data/ActorInteractionTypes.h"
#include "GameFramework/Actor.h"
#include "Interfaces/GameplayTagActor.h"
#include "NpcBaseInteractiveActor.generated.h"

class ABaseCharacter;

// TODO interaction counter
UCLASS()
class ALUMCOCKSGJ_API ANpcBaseInteractiveActor : public AActor, public INpcInteractableActor, public IGameplayTagAssetInterface,
	public IGameplayTagActor
{
	GENERATED_BODY()

public:
	ANpcBaseInteractiveActor();

	virtual bool TryStartNpcInteraction_Implementation(ABaseCharacter* Character, const FGameplayTag& InteractionTag = FGameplayTag()) override;
	virtual bool StopNpcInteraction_Implementation(ABaseCharacter* Character, bool bInterupted) override;
	virtual bool GetInteractionLocation(const FGameplayTag& InteractionTag, FVector& OutLocation) override;
	
	virtual bool IsNpcInteractionAvailable_Implementation(const FGameplayTag& InteractionTag = FGameplayTag()) const override;

	virtual bool GetNpcActiveInteractionTag(ABaseCharacter* Npc, FGameplayTag& OutTag) const override;	
	
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual void ChangeGameplayTags(const FGameplayTagContainer& TagsContainer, bool bAdd) override;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UAIPerceptionStimuliSourceComponent* StimulusSourceComponent;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(MakeEditWidget))
	// TArray<FVector> InteractionPositions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(MakeEditWidget))
	TArray<FActorInteractionOption> ActorInteractionOptions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(MakeEditWidget))
	FGameplayTag DefaultInteractionTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int MaxInteractionsAtOnce = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer InteractableActorTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bQuestInteractionActor = false;
	
	UPROPERTY()
	TMap<int, FRunningInteractionData> InteractingCharacters;
 	
	UFUNCTION(BlueprintCallable)
	void BeginInteraction(int InteractionOptionIndex, const FRunningInteractionData& NewInteraction);

	UFUNCTION(BlueprintCallable)
	void FinishInteraction(ABaseCharacter* Character);

	void RegisterNewInteraction(int InteractionOptionIndex, const FRunningInteractionData& NewInteraction);
	virtual int GetAvailableInteractionPosition(const FGameplayTag& InteractionTag) const;
	int GetInteractionPosition(const ABaseCharacter* Character);
	void UnregisterInteraction(ABaseCharacter* Character);
	
	virtual void ClearInteractionState(ABaseCharacter* Character);
	
	FGameplayTag GetValidInteractionTag(const FGameplayTag& InteractionTag) const;

	int InteractionCounter = 0;
	virtual void OnInteractionCharacterDied(const ABaseCharacter* InteractionCharacter);
};
