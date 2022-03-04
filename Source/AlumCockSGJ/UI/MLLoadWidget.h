// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MLLoadWidget.generated.h"

class UButton;

UCLASS()
class ALUMCOCKSGJ_API UMLLoadWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* LoadButton;
    virtual void NativeOnInitialized() override;

private:
    UFUNCTION()
    void OnLoad();
};