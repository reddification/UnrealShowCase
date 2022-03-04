// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu/MLMenuPlayerController.h"

void AMLMenuPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetInputMode(FInputModeUIOnly());
    bShowMouseCursor = true;
}

