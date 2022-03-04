#pragma once

#include "DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"

#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG

static TWeakObjectPtr<UDebugSubsystem> DebugSubsystem = nullptr;

const UDebugSubsystem* GetDebugSubsystem(UWorld* World)
{
	if (!DebugSubsystem.Get())
	{
		DebugSubsystem = UGameplayStatics::GetGameInstance(World)->GetSubsystem<UDebugSubsystem>();
	}

	return DebugSubsystem.Get();
}

#endif	
