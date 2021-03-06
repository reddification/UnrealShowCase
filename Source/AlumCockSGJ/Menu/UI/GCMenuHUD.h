// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GCMenuHUD.generated.h"

/**
 *
 */
UCLASS()
class ALUMCOCKSGJ_API AGCMenuHUD : public AHUD
{
    GENERATED_BODY()
protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> MenuWidgetClass;

    virtual void BeginPlay() override;
};
