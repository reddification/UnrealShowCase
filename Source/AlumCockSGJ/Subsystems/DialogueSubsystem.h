#pragma once

#include "CoreMinimal.h"
#include "Data/DialogueParticipant.h"
#include "Data/Quests/QuestDialogue.h"
#include "DialogueSubsystem.generated.h"

struct FNpcPlayerInteractionDTR;
UCLASS()
class ALUMCOCKSGJ_API UDialogueSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void RegisterPlayerController(class ABasePlayerController* NewController);

	bool StartDialogue(const FNpcPlayerInteractionDTR* InteractionData,
		ABaseCharacter* InitialNpc, FNpcDTR* InitialNpcDTR);
	bool StartAutonomousDialogue(const FQuestDialogue& Dialogue);

	void InterruptDialogue(const ABaseCharacter* InterruptingParticipant);
	void FinishDialogue();
	void StartNextLine();
	void SkipLine();

private:
	UPROPERTY()
	ABasePlayerController* PlayerController;

	TArray<FQuestDialogueLine> CurrentDialogue;

	int CurrentLineIndex = -1;
	int TotalDialogueLines = -1;
	bool bLocking = false;
	bool bDialogueRunning = false;
	TArray<FDialogueParticipant> Participants;
	FTimerHandle DialogueLineTimer;

	bool TryStartDialogue(const FQuestDialogue& NewDialogue);
};
