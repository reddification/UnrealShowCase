// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GCLoadLastSaveWidget.generated.h"

/**
 * 
 */
class UButton;

UCLASS()
class ALUMCOCKSGJ_API UGCLoadLastSaveWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
    UPROPERTY(meta = (BindWidget))
    UButton* LoadLastSaveButton;

    virtual void NativeOnInitialized() override;

private:
    UFUNCTION()
    void OnLoadLastSave();
};