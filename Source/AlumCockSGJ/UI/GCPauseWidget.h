// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GCPauseWidget.generated.h"

/**
 *
 */

class UButton;
class UTextBlock;

UCLASS()
class ALUMCOCKSGJ_API UGCPauseWidget : public UUserWidget
{
    GENERATED_BODY()
  
protected:
    UPROPERTY(meta = (BindWidget))
    UButton* ClearPauseButton;
    UPROPERTY(meta = (BindWidget))
    
    UButton* ExitButton;
    virtual void NativeOnInitialized() override;

private:
    UFUNCTION()
    void OnClearPause();
    UFUNCTION()
    void OnExit();
};
