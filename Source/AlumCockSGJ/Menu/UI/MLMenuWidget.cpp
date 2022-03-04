#include "Menu/UI/MLMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Game/MLGameInstance.h"

DEFINE_LOG_CATEGORY_STATIC(LogMLMenuWidget, All, All);

void UMLMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (StartGameButton)
    {
        StartGameButton->OnClicked.AddDynamic(this, &UMLMenuWidget::OnStartGame);
    }

    if (SettingsButton)
    {
        SettingsButton->OnClicked.AddDynamic(this, &UMLMenuWidget::OnSettings);
    }

    if (AboutButton)
    {
        AboutButton->OnClicked.AddDynamic(this, &UMLMenuWidget::OnAbout);
    }
}


void UMLMenuWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
    if (!GetWorld())
        return;

    if (Animation != HideAnimation)
    {
        return;
    }

    const auto MLGameInstance = GetWorld()->GetGameInstance<UMLGameInstance>();
    if (!MLGameInstance)
        return;

    if (MLGameInstance->GetStartupLevelName().IsNone())
    {
        UE_LOG(LogMLMenuWidget, Error, TEXT("Level name is NONE"));
        return;
    }

    UGameplayStatics::OpenLevel(this, MLGameInstance->GetStartupLevelName());
}

void UMLMenuWidget::OnStartGame()
{
    PlayAnimation(HideAnimation);
}

void UMLMenuWidget::OnSettings()
{
}

void UMLMenuWidget::OnAbout()
{
}
