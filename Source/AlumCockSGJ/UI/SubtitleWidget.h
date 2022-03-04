#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UObject/Object.h"
#include "SubtitleWidget.generated.h"

class UTextBlock;
UCLASS()
class ALUMCOCKSGJ_API USubtitleWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Setup(const FText& SpeakerName, const FText& SpeechText, float Duration);

protected:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* SpeakerNameTextblock;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* SpeechTextblock;

private:
	FTimerHandle AutodestroyTimer;

	void Autodestroy();
};
