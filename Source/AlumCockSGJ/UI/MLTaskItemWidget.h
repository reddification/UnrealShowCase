// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Game/MLCoreTypes.h"
#include "MLTaskItemWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class ALUMCOCKSGJ_API UMLTaskItemWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void SynchronizeProperties() override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FSubtaskItem SubtaskItem;
private:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TitleText;
    UPROPERTY(meta = (BindWidget))
    UImage* ImageLine;
    UPROPERTY(meta = (BindWidget))
    UImage* ImageComplited;
};
