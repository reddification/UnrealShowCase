// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSubsystemUtils.h"
#include "SaveSubsystemTypes.h"
#include "SaveSubsystemInterface.h"

void USaveSubsystemUtil::BroadcastOnLevelDeserialized(ULevel* Level)
{
    UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystemUtils::BroadcastOnLevelDeserialized(): %s"), *GetNameSafe(Level));
    if (!IsValid(Level))
    {
        return;
    }

    for (AActor* Actor : Level->Actors)
    {
        if (IsValid(Actor) && Actor->Implements<USaveSubsystemInterface>())
        {
            ISaveSubsystemInterface::Execute_OnLevelDeserialized(Actor);
        }
    }
}
