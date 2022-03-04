// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SoundDTR.generated.h"

USTRUCT()
struct ALUMCOCKSGJ_API FSoundDTR : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "FMOD")
    FText Name;
    UPROPERTY(EditAnywhere, Category = "FMOD")
    class UFMODEvent* FmodSoundEvent;
    UPROPERTY(EditAnywhere, Category = "FMOD")
    float ObjectType;
    UPROPERTY(EditAnywhere, Category = "FMOD")
    float Duration = 5.0f;
};
