#pragma once

#include "SoakingSettings.generated.h"

UCLASS()
class ALUMCOCKSGJ_API USoakingSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	// X axis - time in seconds, Y axis - 0..1 value how soaked the character is
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveFloat* SoakingCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SoakingRate = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DryingRate = 0.3f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SoakingParameterName = FName("Soakness");
};