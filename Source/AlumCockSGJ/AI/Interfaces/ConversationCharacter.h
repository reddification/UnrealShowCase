#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ConversationCharacter.generated.h"

UINTERFACE()
class UConversationCharacter : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API IConversationCharacter
{
	GENERATED_BODY()

public:
	virtual bool RequestConversation() { return false; }
	virtual void FinishConversation() { }
};
