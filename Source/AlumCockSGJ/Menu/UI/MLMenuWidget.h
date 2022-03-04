#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/MLBaseWidget.h"
#include "MLMenuWidget.generated.h"

class UButton;

UCLASS()
class ALUMCOCKSGJ_API UMLMenuWidget : public UMLBaseWidget
{
    GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* StartGameButton;

    UPROPERTY(meta = (BindWidget))
    UButton* SettingsButton;

    UPROPERTY(meta = (BindWidget))
    UButton* AboutButton;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* HideAnimation;

    virtual void NativeOnInitialized() override;
    virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;


private:
    UFUNCTION()
    void OnStartGame();

    UFUNCTION()
    void OnSettings();

    UFUNCTION()
    void OnAbout();
};
