#pragma once
#include "GameplayTagContainer.h"
#include "NpcDialogueLine.h"
#include "Engine/DataTable.h"

#include "ConversationSettings.generated.h"

USTRUCT(BlueprintType)
struct FNpcConversation
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer AtWorldState;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FNpcDialogueLine> DialogueLines;
};

USTRUCT(BlueprintType)
struct FConversationScript
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(RowType="NpcDTR"))
	FDataTableRowHandle WithCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FNpcConversation> ConversationsScripts;
};


struct FConversationOption
{
	const FNpcConversation* Conversation = nullptr;
	int WorldStateSimilarityScore = 0;

	bool operator < (const FConversationOption& Other) const
	{
		return WorldStateSimilarityScore > Other.WorldStateSimilarityScore;
	}
};

UCLASS()
class UNpcConversationSettings : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagQuery ConversationTagFilter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ConversationRange = 500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FConversationScript> Conversations;
};
