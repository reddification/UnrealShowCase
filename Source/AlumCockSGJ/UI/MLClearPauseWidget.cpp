// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MLClearPauseWidget.h"

#include "Gameframework/GameModeBase.h"
#include "Components/Button.h"


void UMLClearPauseWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (ClearPauseButton)
    {
        ClearPauseButton->OnClicked.AddDynamic(this, &UMLClearPauseWidget::OnClearPause);
    }
 
}

void UMLClearPauseWidget::OnClearPause()
{
    if (!GetWorld() && !GetWorld()->GetAuthGameMode())
        return;

    GetWorld()->GetAuthGameMode()->ClearPause();
}