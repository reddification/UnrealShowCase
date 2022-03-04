#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseInteractableInfoWidget.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBaseInteractableInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetName(const FText& Text);

protected:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* NameTextblock;
};
