#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DialogueParticipantManager.generated.h"

UINTERFACE()
class UDialogueParticipantManager : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API IDialogueParticipantManager
{
	GENERATED_BODY()

public:
	virtual void OnDialogueStarted(bool bFollowPlayer) {}
	virtual void OnDialogueEnded() {}
};
