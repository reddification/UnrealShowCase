#pragma once

#include "ConversationSettings.h"
#include "FeelingSettings.h"
#include "NpcBehaviorSettings.h"
#include "NpcHumanoidStateSettings.h"
#include "Data/DataAssets/ModularCharacterOutfitSettings.h"
#include "Data/DataAssets/Characters/CharacterInteractionOptions.h"
#include "Data/Entities/SpawnItemDescription.h"
#include "Engine/DataTable.h"
#include "NpcDTR.generated.h"

USTRUCT(BlueprintType)
struct FGestureItem
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName AttachmentSocket = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* StaticMesh = nullptr;
};

USTRUCT(BlueprintType)
struct FNpcReaction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundCue* VoiceLine;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText SpeechText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* Gesture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FGameplayTag, FGestureItem> GestureItems;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = 0.f, ClampMin = 0.f))
	float SpeechLoudness = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = 0.f, ClampMin = 0.f))
	float SpeechRangeAI = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = 0.f, ClampMin = 0.f))
	float Duration = 2.f;
};

USTRUCT(BlueprintType)
struct FNpcReactions
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FNpcReaction> NpcReactions;
};

USTRUCT(BlueprintType)
struct FInteractionBehavior
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InteractionTypeTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FNpcBehaviorActivityDescriptor NpcBehaviorActivityDescriptor;
};

UCLASS(BlueprintType)
class UNpcReactionsSettings : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FGameplayTag, FNpcReactions> NpcReactions;
};

USTRUCT(BlueprintType)
struct ALUMCOCKSGJ_API FNpcDTR : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Base")
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Base")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Base")
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Base")
	bool bUnique = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Base")
	UModularCharacterOutfitSettings* ModularCharacterOutfitSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base", meta=(MustImplement=DatatableInitializablePawn))
	TSubclassOf<ABaseCharacter> SpawnClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Behaviors")
	UBehaviorTree* BaseBehavior;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Behaviors")
	TArray<FNpcActivityRestBehavior> RestBehaviors;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Behaviors")
	TArray<FNpcActivityWorkBehavior> WorkBehaviors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Behaviors")
	TArray<FInteractionBehavior> InteractionBehaviors;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Behaviors")
	TSubclassOf<UNavigationQueryFilter> DefaultNavigationQueryFilterClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Reactions")
	TArray<FNpcBehaviorReactionDescriptor> ReactionsBehaviors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Reactions")
	UNpcFeelingSettingsBundle* FeelingSettingsBundle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Reactions")
	UNpcReactionsSettings* Reactions;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quests")
	TArray<FSpawnItemDescription> SpawnItemsAfterDeath;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="States")
	TMap<FGameplayTag, UBaseNpcStateSettings*> NpcStates;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="States")
	FGameplayTag InitialState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interactions")
	UCharacterInteractionOptions* CharacterInteractionOptions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interactions", meta=(RequiredAssetDataTags="RowStructure=NpcPlayerInteractionDTR"))
	UDataTable* NpcPlayerInteractions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interactions")
	UNpcConversationSettings* ConversationSettings;

};
