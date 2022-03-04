// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Game/MLCoreTypes.h"
#include "MLQuestPageWidget.generated.h"

class UVerticalBox;
UCLASS()
class ALUMCOCKSGJ_API UMLQuestPageWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void SynchronizeProperties() override;
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FJournalModel Model;
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* VBox;
};
