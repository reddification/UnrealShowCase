#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ModularCharacterOutfitSettings.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UModularCharacterOutfitSettings : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USkeletalMesh* Torso;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USkeletalMesh* Pants;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USkeletalMesh* Hair;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USkeletalMesh* Face;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USkeletalMesh* Hat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USkeletalMesh* Boots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USkeletalMesh* Features;
};
