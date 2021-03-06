#pragma once
#include "WallrunSettings.generated.h"

class UCurveFloat;

UCLASS(BlueprintType)
class ALUMCOCKSGJ_API UWallrunSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.5", UIMin="0.5"))
	float MaxTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="200", UIMin="200"))
	float MaxSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="500", UIMin="500"))
	float JumpOffSpeed = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="200", UIMin="200"))
	float MaxDeltaHeight = 300.f;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", UIMin="0"))
	float HandTraceActorZOffset = 75.f;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", UIMin="0"))
	float FeetTraceActorZOffset = 60.f;	

	/*
	 * Min Velocity Z-component value below which wallrun won't start
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinVelocityZ = -350.f;	

	/*
	 * Min speed character must have to start wallrun
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", UIMin="0"))
	float MinRequiredSpeed = 200.f;
	
	/*
	 * Starting from this distance between character's capsule and a wall the wallrun will begin 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", UIMin="0"))
	float WallDistance = 15.f;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", UIMin="0"))
	float RotationInterpSpeed = 10.f;
	
	/*
	* Ratio-based, 0..1 on X and -1 to 1 on Y
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveFloat* WallrunHeightDynamicsFromGroundCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveFloat* WallrunHeightDynamicsFromAirCurve = nullptr;

	/*
	 * 0..1 on X, Y value will be multiplied by WallrunMaxSpeed 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveFloat* WallrunSpeedDynamicsCurve = nullptr;
};