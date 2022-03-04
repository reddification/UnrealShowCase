// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BehaviorTree.h"
#include "Engine/DataTable.h"
#include "DogDTR.generated.h"

USTRUCT()
struct ALUMCOCKSGJ_API FDogDTR : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText Name;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UTexture2D* Icon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UBehaviorTree* BaseBehavior;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UBehaviorTree* FollowThePlayerBehavior;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(RowType="QuestDTR"))
    TArray<FDataTableRowHandle> BeginQuests;
};
