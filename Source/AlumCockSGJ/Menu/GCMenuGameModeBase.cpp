// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu/GCMenuGameModeBase.h"

#include "Menu/GCMenuPlayerController.h"
#include "Menu/UI/GCMenuHUD.h"

AGCMenuGameModeBase::AGCMenuGameModeBase()
{

    PlayerControllerClass = AGCMenuPlayerController::StaticClass();
    HUDClass = AGCMenuHUD::StaticClass();
}

