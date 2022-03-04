#pragma once
#include "AI/EQS/Tests/EnvQueryTest_HasGameplayTags.h"
#include "Data/Quests/QuestAction.h"
#include "Data/Quests/QuestDialogue.h"
#include "Data/Quests/QuestRequirements.h"

#include "NpcPlayerInteractionDTR.generated.h"

USTRUCT(BlueprintType)
struct FNpcPlayerInteractionDTR : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FQuestRequirements PlayerRequirements;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagQuery NpcGameplayStateRequired;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FQuestDialogue Dialogue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bLocking = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bFollowPlayer = false;
};
