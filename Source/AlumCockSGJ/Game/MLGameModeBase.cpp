#include "Game/MLGameModeBase.h"

#include "GenericTeamAgentInterface.h"
#include "AI/Data/AITypesGC.h"

void AMLGameModeBase::StartPlay()
{
    Super::StartPlay();
    FGenericTeamId::SetAttitudeSolver(GCTeamAttitudeSolver);
}