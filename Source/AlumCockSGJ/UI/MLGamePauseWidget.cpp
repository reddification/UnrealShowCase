#include "UI/MLGamePauseWidget.h"

#include "Gameframework/GameModeBase.h"
#include "Components/Button.h"

void UMLGamePauseWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UMLGamePauseWidget:: OnClearPause()
{
    if (!GetWorld() && !GetWorld()->GetAuthGameMode())
        return;

    GetWorld()->GetAuthGameMode()->ClearPause();
}