// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MLBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class ALUMCOCKSGJ_API UMLBaseWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void Show();
protected:
    UPROPERTY(meta=(BindWidgetAnim), Transient)
    UWidgetAnimation* ShowAnimation;
};
