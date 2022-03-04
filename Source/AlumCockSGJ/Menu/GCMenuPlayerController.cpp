// Fill out your copyright notice in the Description page of Project Settings.

#include "Menu/GCMenuPlayerController.h"

void AGCMenuPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetInputMode(FInputModeUIOnly());
    bShowMouseCursor = true;
}