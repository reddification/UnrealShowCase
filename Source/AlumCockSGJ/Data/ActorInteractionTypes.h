#pragma once

#include "GameplayTagContainer.h"
#include "ActorInteractionTypes.generated.h"

class ABaseCharacter;

USTRUCT(BlueprintType)
struct FActorInteractionOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(MakeEditWidget))
	FVector InteractionPosition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(MakeEditWidget))
	bool bUseCustomRotation = false;
	
	// TODO utilize
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition = "bUseCustomRotation"))
	FRotator InteractionRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag InteractionTag;
};

USTRUCT(BlueprintType)
struct FRunningInteractionData
{
	GENERATED_BODY()

	FRunningInteractionData()
		: Character(nullptr), Montage(nullptr)
	{ }
	
	FRunningInteractionData(ABaseCharacter* Character)
		: Character(Character), Montage(nullptr)
	{  }
	
	FRunningInteractionData(ABaseCharacter* Character, UAnimMontage* Montage)
		: Character(Character), Montage(Montage)
	{  }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ABaseCharacter* Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage;

	mutable FDelegateHandle DeathDelegateHandle;
	
	void Reset()
	{
		Character = nullptr;
		Montage = nullptr;
		DeathDelegateHandle.Reset();
	}

	bool IsActive() const { return Character != nullptr; }
};