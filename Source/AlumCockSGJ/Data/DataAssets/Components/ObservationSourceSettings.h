#pragma once

#include "ObservationSourceSettings.generated.h"

UCLASS()
class UObservationSourceSettings : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ObservationIntervalMilliseconds = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ObservationDistanceThreshold = 10000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PlayerViewDirectionDotProductThreshold = 0.95;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDrawDebug = false;
};
