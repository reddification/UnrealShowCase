// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/GCPauseWidget.h"
#include "Gameframework/GameModeBase.h"
#include "Components/Button.h"
#include "GCGameMode.h"

void UGCPauseWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (ClearPauseButton)
    {
        ClearPauseButton->OnClicked.AddDynamic(this, &UGCPauseWidget::OnClearPause);
    }

    if (ExitButton)
    {
        ExitButton->OnClicked.AddDynamic(this, &UGCPauseWidget::OnExit);
    }
}

void UGCPauseWidget::OnClearPause()
{
    if (!GetWorld() && !GetWorld()->GetAuthGameMode())
        return;

    GetWorld()->GetAuthGameMode()->ClearPause();
}


void UGCPauseWidget::OnExit()
{
    UGameViewportClient* const Viewport = GetWorld()->GetGameInstance()->GetGameViewportClient();
    if (Viewport)
    {
        Viewport->ConsoleCommand("quit");
    }
}