// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MLExitWidget.h"

#include "Gameframework/GameModeBase.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UMLExitWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (ExitButton)
    {
        ExitButton->OnClicked.AddDynamic(this, &UMLExitWidget::OnExit);
    }
}


void UMLExitWidget::OnExit()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}