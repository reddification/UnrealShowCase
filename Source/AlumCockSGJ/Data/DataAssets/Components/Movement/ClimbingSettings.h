#pragma once

#include "ClimbingSettings.generated.h"

UCLASS(BlueprintType)
class ALUMCOCKSGJ_API UClimbingSettings : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float MaxClimbingSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float ClimbingBrakingDeceleration = 4096.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float ClimbingOffset = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float ClimbingDetachTopOffset = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float ClimbingDetachBottomOffset = 90.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float JumpOffClimbableSpeed = 500.f;
};