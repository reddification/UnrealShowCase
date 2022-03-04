#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/CommonWrappers.h"
#include "UObject/Object.h"
#include "CharacterInteractionOptions.generated.h"

UCLASS(BlueprintType)
class ALUMCOCKSGJ_API UCharacterInteractionOptions : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FGameplayTag, FMontagesWrapper> InteractionMontages;
};
