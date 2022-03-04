#pragma once
#include "HumanoidCharacterDataAsset.h"
#include "PlayerCharacterDataAsset.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UPlayerCharacterDataAsset : public UHumanoidCharacterDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	float BaseTurnRate = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	float BaseLookUpRate = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprint")
	float SprintSpringArmOffset = 420.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint")
	UCurveFloat* SprintSpringArmTimelineCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aiming")
	UCurveFloat* AimFovAdjustmentCurve = nullptr;
};