﻿#pragma once

#include "IKSettings.generated.h"

UCLASS(BlueprintType)
class ALUMCOCKSGJ_API UIKSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RightFootSocketName = "RightFootSocketIK";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName LeftFootSocketName = "LeftFootSocketIK";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RightHeelSocketName = "RightHeelSocket";
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RightToesSocketName = "RightToesSocket";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName LeftHeelSocketName = "LeftHeelSocket";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName LeftToesSocketName = "LeftToesSocket";
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0, UIMin = 0))
	float IKInterpSpeed = 15;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0, UIMin = 0))
	float IKTraceDistance = 60;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0, UIMin = 0))
	float IKTraceDistanceCrouch = 30;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0, UIMin = 0))
	float TraceExtend = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0, UIMin = 0))
	float FootLength = 30;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0, UIMin = 0))
	float FootWidth = 7;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0, UIMin = 0))
	float MinFootPitchForElevationTrace = 35;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0, UIMin = 0))
	float MaxKneeOutwardExtend = 40;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0, UIMin = 0))
	float MaxFootElevation = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDrawDebug = false;
};
