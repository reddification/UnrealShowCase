#include "GCGameMode.h"

#include "EngineUtils.h"
#include "GenericTeamAgentInterface.h"
#include "CommonConstants.h"
#include "AI/Data/AITypesGC.h"

void AGCGameMode::StartPlay()
{
    Super::StartPlay();
    FGenericTeamId::SetAttitudeSolver(GCTeamAttitudeSolver);
    SetMatchStatus(EGCMatchStatus::InProgress);
}

void AGCGameMode::OnGameOver()
{
    /* for (auto Pawn : TActorRange<APawn>(GetWorld()))
    {
        if (Pawn)
        {
            Pawn->TurnOff();
            Pawn->DisableInput(nullptr);
        }
    }*/
    SetMatchStatus(EGCMatchStatus::GameOver);
}

bool AGCGameMode::SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate)
{
    const auto PauseSet = Super::SetPause(PC, CanUnpauseDelegate);
    if (PauseSet)
    {
        SetMatchStatus(EGCMatchStatus::Pause);
    }
    return PauseSet;
}

void AGCGameMode::SetMatchStatus(EGCMatchStatus Status)
{
    if (MatchStatus == Status)
        return;

    MatchStatus = Status;
    OnMatchStatusChanged.Broadcast(MatchStatus);
}

bool AGCGameMode::ClearPause()
{
    const auto PauseCleared = Super::ClearPause();

    if (PauseCleared)
    {
        SetMatchStatus(EGCMatchStatus::InProgress);
    }
    return PauseCleared;
}
