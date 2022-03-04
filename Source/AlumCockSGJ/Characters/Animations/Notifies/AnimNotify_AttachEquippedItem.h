// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAnimNotify.h"
#include "AnimNotify_AttachEquippedItem.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UAnimNotify_AttachEquippedItem : public UBaseAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
