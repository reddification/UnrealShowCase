#include "UI/MLGameHUD.h"

#include "UI/GCHUD.h"
#include "Engine/Canvas.h"
#include "Game/MLGameModeBase.h"
#include "UI/MLBaseWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogMLGameHUD, All, All);

void AMLGameHUD::DrawHUD()
{
    Super::DrawHUD();
}

void AMLGameHUD::PrepareWidgets()
{
    if (bInitialized)
        return;
    
    GameWidgets.Add(EMLGameState::InProgress, CreateWidget<UMLBaseWidget>(GetWorld(), PlayerHUDWidgetClass));
    GameWidgets.Add(EMLGameState::Pause, CreateWidget<UMLBaseWidget>(GetWorld(), PauseWidgetClass));
    GameWidgets.Add(EMLGameState::GameOver, CreateWidget<UMLBaseWidget>(GetWorld(), GameOverWidgetClass));
    GameWidgets.Add(EMLGameState::Journal, CreateWidget<UMLBaseWidget>(GetWorld(), JournalWidgetClass));

    for (auto GameWidgetPair : GameWidgets)
    {
        const auto GameWidget = GameWidgetPair.Value;
        if (!GameWidget)
            continue;
        
        GameWidget->AddToViewport();
        GameWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    const auto GameMode = Cast<AMLGameModeBase>(GetWorld()->GetAuthGameMode());
    if (GameMode)
        GameMode->OnGameStateChanged.AddUObject(this, &AMLGameHUD::OnGameStateChanged);

    bInitialized = true;
    return;
}

void AMLGameHUD::BeginPlay()
{
    Super::BeginPlay();
    PrepareWidgets();
}

UPlayerHUDWidget* AMLGameHUD::GetPlayerHUD()
{
    PrepareWidgets();
    return Cast<UPlayerHUDWidget>(GameWidgets[EMLGameState::InProgress]);
}

void AMLGameHUD::DrawCrossHair() {}

void AMLGameHUD::OnGameStateChanged(EMLGameState State)
{
    if (CurrentWidget)
    {
        CurrentWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    if (GameWidgets.Contains(State))
    {
        CurrentWidget = GameWidgets[State];
    }

    if (CurrentWidget)
    {
        CurrentWidget->SetVisibility(ESlateVisibility::Visible);
        CurrentWidget->Show();
    }

    UE_LOG(LogMLGameHUD, Display, TEXT("Game state changed: %s"), *UEnum::GetValueAsString(State));
}