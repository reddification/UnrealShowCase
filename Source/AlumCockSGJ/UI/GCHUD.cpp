// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/GCHUD.h"
#include "Engine/Canvas.h"
#include "Blueprint/UserWidget.h"
#include "GCGameMode.h"

DEFINE_LOG_CATEGORY_STATIC(LogGCGameHUD, All, All);

void AGCHUD::DrawHUD()
{
    Super::DrawHUD();
}

void AGCHUD::BeginPlay()
{
    Super::BeginPlay();

    GameWidgets.Add(EGCMatchStatus::InProgress, CreateWidget<UUserWidget>(GetWorld(), PlayerHUDWidgetClass));
    GameWidgets.Add(EGCMatchStatus::Pause, CreateWidget<UUserWidget>(GetWorld(), PauseWidgetClass));
    GameWidgets.Add(EGCMatchStatus::GameOver, CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass));

    for (auto GameWidgetPair : GameWidgets)
    {
        const auto GameWidget = GameWidgetPair.Value;
        if (!GameWidget)
            continue;
        GameWidget->AddToViewport();
        GameWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    if (GetWorld())
    {
        const auto GameMode = Cast<AGCGameMode>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            GameMode->OnMatchStatusChanged.AddUObject(this, &AGCHUD::OnMatchStatusChanged);
        }
    }
}

void AGCHUD::DrawCrossHair() {}
void AGCHUD::OnMatchStatusChanged(EGCMatchStatus Status)
{
    if (CurrentWidget)
    {
        CurrentWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    if (GameWidgets.Contains(Status))
    {
        CurrentWidget = GameWidgets[Status];
    }

    if (CurrentWidget)
    {
        CurrentWidget->SetVisibility(ESlateVisibility::Visible);
    }

    UE_LOG(LogGCGameHUD, Display, TEXT("Match status changed: %s"), *UEnum::GetValueAsString(Status));
}