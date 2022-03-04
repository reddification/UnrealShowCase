#pragma once

#include "CoreMinimal.h"
#include "BaseInteractableInfoWidget.h"
#include "CharacterInfoWidget.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UCharacterInfoWidget : public UBaseInteractableInfoWidget
{
	GENERATED_BODY()
	
public:
	void SetHealthPercent(float Percent);
	
protected:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	float PercentVisisbleThreshold = 0.8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	float PercentColorThreshold = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FLinearColor GoodColor = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FLinearColor BadColor = FLinearColor::Red;
};
