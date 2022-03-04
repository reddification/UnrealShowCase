#pragma once

#include "CoreMinimal.h"
#include "AI/Data/NpcDTR.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Data/Quests/QuestDialogue.h"
#include "DialogueWidget.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// bool StartDialogue(const FQuestDialogue& NewDialogue);
	// bool IsShowingDialogue() const;
	// void ShowNextLine();
	void ShowDialogueLine(const FQuestDialogueLine& Line, const FNpcDTR* Participant) const;
	
protected:
	UPROPERTY(meta = (BindWidget))
	UImage* SpeakerImageWidget;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* TextScrollBoxWidget;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SpeakerNameWidget;
    
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SpeechTextWidget;


private:
	// FTimerHandle DialogueLineTimer;
	// FQuestDialogue Dialogue;
	
	// int CurrentLineIndex = -1;
	// int TotalLines = 0;
	// TArray<FNpcDTR*> Participants; 
	// void ResetDialogue();
};
