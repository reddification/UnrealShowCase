#pragma once

#include "CoreMinimal.h"
#include "MLBaseWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetSwitcher.h"
#include "MLGamePauseWidget.generated.h"

class UButton;

class UWidgetSwitcher;

UCLASS()
class ALUMCOCKSGJ_API UMLGamePauseWidget : public UMLBaseWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeOnInitialized() override;

private:
    UFUNCTION()
    void OnClearPause();
};