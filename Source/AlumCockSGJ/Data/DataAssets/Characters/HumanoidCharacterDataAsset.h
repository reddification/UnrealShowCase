#pragma once
#include "BaseCharacterDataAsset.h"
#include "Data/Movement/MantlingSettings.h"
#include "HumanoidCharacterDataAsset.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UHumanoidCharacterDataAsset : public UBaseCharacterDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement|Mantling")
	FMantlingSettings MantleHighSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement|Mantling")
	FMantlingSettings MantleLowSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement|Mantling", meta=(ClampMin=0.f, UIMin=0.f))
	float MantleLowMaxHeight = 135.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement|Zipline")
	FName ZiplineHandPositionSocketName = FName("zipline_hand_position");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanMantle = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanWallrun = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanZipline = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanSlide = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanClimb = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanCrouch = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanCrawl = false;
};