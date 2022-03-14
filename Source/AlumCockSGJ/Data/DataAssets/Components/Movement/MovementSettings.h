#pragma once

#include "MovementSettings.generated.h"

UCLASS()
class UMovementSettings : public UDataAsset
{
	GENERATED_BODY()
public:
	
#pragma region SPEED UPROPERTIES
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Speed", meta=(ClampMin=0, UIMin=0))
	float SprintSpeed = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Speed", meta=(ClampMin=0, UIMin=0))
	float DefaultAimingSpeed = 300.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Speed", meta=(ClampMin=0, UIMin=0))
	float OutOfStaminaSpeed = 200.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Speed", meta=(ClampMin=0, UIMin=0))
	float CrawlSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Speed", meta=(ClampMin=0, UIMin=0))
	float WalkingSpeed = 150.f;
	
#pragma endregion SPEED UPROPERTIES

#pragma region CAPSULE
	
	UPROPERTY(Category="Character Movement|Swimming", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float SwimmingCapsuleHalfHeight = 50.f;

	UPROPERTY(Category="Character Movement|Swimming", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float SwimmingCapsuleRadius = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=0, UIMin=0))
	float ProneCapsuleRadius = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=0, UIMin=0))
	float ProneCapsuleHalfHeight = 40.0f;
	
#pragma endregion CAPSULE
};
