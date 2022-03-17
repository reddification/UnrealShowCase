#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Components/ActorComponent.h"
#include "NpcPlayerInteractionComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UNpcPlayerInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	bool StartInteractWithPlayer(ABaseCharacter* OtherCharacter);
	void ResetInteractionState() { bInteracting = false; }
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagQuery UnableToInteractInStates;
	
	UPROPERTY()
	ABaseCharacter* OwnerCharacter;

	UPROPERTY()
	TScriptInterface<class INpcCharacter> NpcCharacter;

private:
	bool bInteracting = false;
};
