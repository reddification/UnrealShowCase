#pragma once

#include "CoreMinimal.h"
#include "AI/Data/ConversationSettings.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "Components/ActorComponent.h"
#include "NpcConversationComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UNpcConversationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	bool RequestConversation(UNpcConversationComponent* OtherConversationComponent);
	void FinishConversation();
	void HandleDialogueLine(const FNpcDialogueLine& DialogueLine);
	void AdvanceConversation();

	const FDataTableRowHandle& GetNpcDTRH() const { return OwnerNpcCharacter->GetNpcDTRH(); }
	bool IsInDialogue() const { return bInDialogue; }

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InDialogueTag;

private:
	void OnCharacterDied(const ABaseCharacter* BaseCharacter);
	void SetConversationState(bool bActive);
	void StartConversation();

	UNpcConversationComponent* OtherConversationComponent;
	FTimerHandle LineTimer;
	
	UPROPERTY()
	ABaseCharacter* Character;
	
	INpcCharacter* OwnerNpcCharacter;

	FConversationOption CurrentConversation;

	int CurrentIndex = 0;
	bool bInDialogue = false;
	bool bDialogueMaster = false;
};
