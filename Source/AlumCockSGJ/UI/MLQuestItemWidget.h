// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Game/MLCoreTypes.h"
#include "MLQuestItemWidget.generated.h"

class UTextBlock;
class UImage;
class UButton;

DECLARE_MULTICAST_DELEGATE_OneParam(FQuestItemActiveEvent, const FName& Name)

UCLASS()
class ALUMCOCKSGJ_API UMLQuestItemWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FLinearColor NormalColor = FLinearColor::Black;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FLinearColor HoveredColor = FLinearColor::Red;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FLinearColor SelectedColor = FLinearColor::Blue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    class USoundCue* ClickSFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FQuestItem QuestItem;
    
    virtual void SynchronizeProperties() override;

    mutable FQuestItemActiveEvent OnQuestItemActiveEvent;
    void SetActive(bool NewActive);
    
protected:
    virtual void NativeOnInitialized() override;
private:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TitleText;
    UPROPERTY(meta = (BindWidget))
    UImage* ImageLine;
    UPROPERTY(meta = (BindWidget))
    UImage* ImageSelected;
    UPROPERTY(meta = (BindWidget))
    UImage* ImageComplited;
   
    UPROPERTY(meta = (BindWidget))
    UButton* MenuButton;

    bool IsActive = false;
    UFUNCTION()
    void OnClick();
    UFUNCTION()
    void OnHovered();
    UFUNCTION()
    void OnUnhovered();
};
