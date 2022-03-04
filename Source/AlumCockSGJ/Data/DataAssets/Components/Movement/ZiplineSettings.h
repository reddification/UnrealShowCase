#pragma once

#include "ZiplineSettings.generated.h"

UCLASS()
class UZiplineSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|Zipline")
	float MinZiplineSpeed = 100.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|Zipline")
	float MaxZiplineSpeed = 1600.0f;
	
};
