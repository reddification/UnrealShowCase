// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MLQuestTitleWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class ALUMCOCKSGJ_API UMLQuestTitleWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void SynchronizeProperties() override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FText DisplayText;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    bool IsComplited = false;
private:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TitleText;
    UPROPERTY(meta = (BindWidget))
    UImage* LineCross;
    UPROPERTY(meta = (BindWidget))
    UImage* CompletionMark;
};
