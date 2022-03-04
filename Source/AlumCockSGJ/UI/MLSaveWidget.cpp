// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MLSaveWidget.h"
#include "Game/MLGameModeBase.h"
#include "Game/MLCoreTypes.h"
#include "Gameframework/GameModeBase.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UMLSaveWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (SaveButton)
    {
        SaveButton->OnClicked.AddDynamic(this, &UMLSaveWidget::OnSave);
    }
}

void UMLSaveWidget::OnSave() {
    /* if (GetWorld())
    {
        const auto GameMode = Cast<AMLGameModeBase>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            GameMode->SetGameState(EMLGameState::Save);
        }
    }
    */

}