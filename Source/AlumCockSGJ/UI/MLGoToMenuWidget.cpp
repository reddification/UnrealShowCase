#include "UI/MLGoToMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Game/MLGameInstance.h"

DEFINE_LOG_CATEGORY_STATIC(LogMLGoToMenuWidget, All, All);

void UMLGoToMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (GoToMenuButton)
    {
        GoToMenuButton->OnClicked.AddDynamic(this, &UMLGoToMenuWidget::OnGoToMenu);
    }
}

void UMLGoToMenuWidget::OnGoToMenu()
{
    if (!GetWorld())
        return;

    const auto GCGameInstance = GetWorld()->GetGameInstance<UMLGameInstance>();
    if (!GCGameInstance)
        return;

    if (GCGameInstance->GetMenuLevelName().IsNone())
    {
        UE_LOG(LogMLGoToMenuWidget, Error, TEXT("Menu level name is NONE"));
        return;
    }

    UGameplayStatics::OpenLevel(this, GCGameInstance->GetMenuLevelName());
}