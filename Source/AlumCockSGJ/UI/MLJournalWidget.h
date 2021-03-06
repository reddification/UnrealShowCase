// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MLQuestItemWidget.h"
#include "Components/VerticalBox.h"
#include "Data/Quests/QuestTypes.h"
#include "UI/MLBaseWidget.h"
#include "Game/MLCoreTypes.h"
#include "MLJournalWidget.generated.h"

class USpacer;
class UButton;
class UHorizontalBox;
class UTextBlock;
class UImage;

UCLASS()
class ALUMCOCKSGJ_API UMLJournalWidget : public UMLBaseWidget
{
    GENERATED_BODY()
public:
    virtual void Show() override;
    
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    TSubclassOf<UUserWidget> QuestTitleClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    TSubclassOf<UUserWidget> QuestItemClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    TSubclassOf<UUserWidget> TaskItemClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FText TextMainQuest;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FText TextAuxiliaryQuest;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FText TextComplitedQuest;

    UPROPERTY(meta = (BindWidget))
    UButton* ClearPauseButton;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* VBox;

    virtual void NativeOnInitialized() override;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* QuestDetailTitle;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextDescription;

    UPROPERTY(meta = (BindWidget))
    UImage* MapImage;
    
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* DetailBox;
    
private:
    // FJournalModel Model;
    FQuestItem ActiveQuest;
    
    TMap<FName, UMLQuestItemWidget*> QuestWidgets;

    UPROPERTY()
    UMLQuestItemWidget* ActiveQuestWidget;

    UFUNCTION()
    void OnClearPause();

    void AddVerticalSpacer() const;
    void ClearWidgets();
    void UpdateContent(const FJournalModel& JournalModel);
    void OnWorldStateChanged(const FGameplayTagContainer& NewWorldState);

    FQuestNavigationGuidance ActiveGuidance;
    FDelegateHandle WorldStateChangeHandle;
    
    void OnChangeQuest(const FName& Name);
};
