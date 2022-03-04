// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MLLoadWidget.h"
#include "Game/MLGameModeBase.h"
#include "Game/MLCoreTypes.h"
#include "Gameframework/GameModeBase.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UMLLoadWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (LoadButton)
    {
        LoadButton->OnClicked.AddDynamic(this, &UMLLoadWidget::OnLoad);
    }
}

void UMLLoadWidget::OnLoad()
{
    /* if (GetWorld())
    {
        const auto GameMode = Cast<AMLGameModeBase>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            GameMode->SetGameState(EMLGameState::Load);
        }
    }
    */
}