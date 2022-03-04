// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MLMenuGameModeBase.h"
#include "Menu/MLMenuPlayerController.h"
#include "Menu/UI/MLMenuHUD.h"

AMLMenuGameModeBase::AMLMenuGameModeBase()
{

    PlayerControllerClass = AMLMenuPlayerController::StaticClass();
    HUDClass = AMLMenuHUD::StaticClass();
}