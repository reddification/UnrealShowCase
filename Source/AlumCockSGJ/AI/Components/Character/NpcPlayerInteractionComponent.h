#pragma once

#include "CoreMinimal.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "Characters/BaseCharacter.h"
#include "Components/ActorComponent.h"
#include "NpcPlayerInteractionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UNpcPlayerInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	bool StartInteractWithCharacter(ABaseCharacter* OtherCharacter);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagQuery UnableToInteractInStates;
	
	UPROPERTY()
	ABaseCharacter* OwnerCharacter;

	UPROPERTY()
	TArray<ABaseCharacter*> InteractionParticipants;
	
	UPROPERTY()
	TScriptInterface<INpcCharacter> NpcCharacter;
};
