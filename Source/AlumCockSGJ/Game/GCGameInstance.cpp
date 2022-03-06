#include "GCGameInstance.h"
#include "DebugSubsystem.h"
#include "NavigationSystem.h"
#include "Actors/Interactive/Environment/QuestLocation.h"
#include "Actors/Interactive/Pickables/BasePickableItem.h"
#include "Data/Entities/WorldItemDTR.h"
#include "Interfaces/DatatableInitializablePawn.h"
#include "Kismet/GameplayStatics.h"

bool UGCGameInstance::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	bool bResult = Super::ProcessConsoleExec(Cmd, Ar, Executor);
	if (!bResult)
	{
		TArray<UGameInstanceSubsystem*> Subsystems = GetSubsystemArray<UGameInstanceSubsystem>();
		for (const auto Subsystem : Subsystems)
		{
			if (Subsystem->ProcessConsoleExec(Cmd, Ar, Executor))
				break;
		}
	}
	
	return bResult;
}
