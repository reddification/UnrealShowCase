// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShake.h"
#include "CameraShakeComponent.generated.h"

/**
 * 
 */
UCLASS()
class ALUMCOCKSGJ_API UCameraShakeComponent : public UMatineeCameraShake
{
	GENERATED_BODY()

public:
	UCameraShakeComponent();
protected:
    UPROPERTY(EditAnywhere)
    float OscillationDurationValue = 0.2f;

    UPROPERTY(EditAnywhere)
    float OscillationBlendInTimeValue = 0.1f;

    UPROPERTY(EditAnywhere)
    float OscillationBlendOutTimeValue = 0.2f;

    UPROPERTY(EditAnywhere)
    float RotOscillationPitchAmplitudeValue = 2.0f;

    UPROPERTY(EditAnywhere)
    float RotOscillationYawAmplitudeValue = 2.0f;

    UPROPERTY(EditAnywhere)
    float LocOscillationXAmplitudeValue = 15.0f;
    
    UPROPERTY(EditAnywhere)
    float LocOscillationXFrequencyValue = 1.0f;

    UPROPERTY(EditAnywhere)
    float LocOscillationZAmplitudeValue = 5.0f;

    UPROPERTY(EditAnywhere)
    float LocOscillationZFrequencyValue = 15.0f;
};
