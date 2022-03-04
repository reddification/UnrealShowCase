#pragma once

#include "DialogueParticipant.generated.h"

class ABaseCharacter;
struct FNpcDTR;

USTRUCT()
struct FDialogueParticipant
{
	GENERATED_BODY()

	FDialogueParticipant(ABaseCharacter* Character, FNpcDTR* NpcDTR)
		: Character(Character), NpcDTR(NpcDTR)
	{  }

	FDialogueParticipant() {  }
	
	UPROPERTY()
	ABaseCharacter* Character = nullptr;

	UAnimMontage* RunningMontage = nullptr;
	
	FNpcDTR* NpcDTR = nullptr;
};
