// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MLMenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ALUMCOCKSGJ_API AMLMenuPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
};
