#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "Interfaces/Interactable.h"
#include "InteractiveQuestActor.generated.h"

UCLASS()
class ALUMCOCKSGJ_API AInteractiveQuestActor : public AActor, public IInteractable, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	AInteractiveQuestActor();
	
	virtual bool InteractWithPlayer(APlayerCharacter* Interactor) override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UQuestGiverComponent* QuestGiverComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USoundCue* InteractSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer GameplayTags;
};
