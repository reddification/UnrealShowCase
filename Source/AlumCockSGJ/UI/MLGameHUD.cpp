#include "UI/MLGameHUD.h"

#include "UI/GCHUD.h"
#include "Engine/Canvas.h"
#include "Game/MLGameModeBase.h"
#include "UI/MLBaseWidget.h"

void AMLGameHUD::DrawHUD()
{
    Super::DrawHUD();
}

void AMLGameHUD::OnChangeMouseCursorEvent(bool bActive)
{
    ChangeMouseCursorEvent.ExecuteIfBound(bActive);
}

void AMLGameHUD::PrepareWidgets()
{
    if (bInitialized)
        return;

    auto World = GetWorld();
    GameWidgets.Add(EMLGameState::Playing, CreateWidget<UMLBaseWidget>(World, PlayerHUDWidgetClass));
    GameWidgets.Add(EMLGameState::Pause, CreateWidget<UMLBaseWidget>(World, PauseWidgetClass));
    GameWidgets.Add(EMLGameState::GameOver, CreateWidget<UMLBaseWidget>(World, GameOverWidgetClass));
    GameWidgets.Add(EMLGameState::Journal, CreateWidget<UMLBaseWidget>(World, JournalWidgetClass));

    for (auto GameWidgetPair : GameWidgets)
    {
        const auto GameWidget = GameWidgetPair.Value;
        if (!GameWidget)
            continue;
        
        GameWidget->AddToViewport();
        GameWidget->SetVisibility(ESlateVisibility::Hidden);
        GameWidget->ChangeMouseCursorEvent.BindUObject(this, &AMLGameHUD::OnChangeMouseCursorEvent);
    }

    bInitialized = true;
}

void AMLGameHUD::BeginPlay()
{
    Super::BeginPlay();
    PrepareWidgets();
    ChangeGameState(EMLGameState::Playing);
}

UPlayerHUDWidget* AMLGameHUD::GetPlayerHUD()
{
    PrepareWidgets();
    return Cast<UPlayerHUDWidget>(GameWidgets[EMLGameState::Playing]);
}

void AMLGameHUD::ChangeGameState(EMLGameState State)
{
    if (CurrentWidget)
        CurrentWidget->Close();

    if (GameWidgets.Contains(State))
    {
        CurrentWidget = GameWidgets[State];
        CurrentWidget->Show();
    }
}

void AMLGameHUD::OpenJournal(APlayerController* PC)
{
    auto GameMode = GetWorld()->GetAuthGameMode();
    if (GameMode)
        GameMode->SetPause(PC);

    ChangeGameState(EMLGameState::Journal);
}
