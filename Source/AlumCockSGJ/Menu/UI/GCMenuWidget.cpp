#include "Menu/UI/GCMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Game/GCGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogGCMenuWidget, All, All);

void UGCMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (StartGameButton)
    {
        StartGameButton->OnClicked.AddDynamic(this, &UGCMenuWidget::OnStartGame);
    }

    if (QuitGameButton)
    {
        QuitGameButton->OnClicked.AddDynamic(this, &UGCMenuWidget::OnQuitGame);
    }
}

void UGCMenuWidget::OnStartGame()
{
    if (!GetWorld())
        return;

    const auto GCGameInstance = GetWorld()->GetGameInstance<UGCGameInstance>();
    if (!GCGameInstance)
        return;

    if (GCGameInstance->GetStartupLevelName().IsNone())
    {
        UE_LOG(LogGCMenuWidget, Error, TEXT("Level name is NONE"));
        return;
    }

    UGameplayStatics::OpenLevel(this, GCGameInstance->GetStartupLevelName());
}

void UGCMenuWidget::OnQuitGame()
{
    UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}