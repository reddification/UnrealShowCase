#include "Game/MLGameModeBase.h"

#include "EngineUtils.h"
#include "GenericTeamAgentInterface.h"
#include "CommonConstants.h"
#include "AI/Data/AITypesGC.h"

void AMLGameModeBase::StartPlay()
{
    Super::StartPlay();
    FGenericTeamId::SetAttitudeSolver(GCTeamAttitudeSolver);
    SetGameState(EMLGameState::InProgress);
}

void AMLGameModeBase::OnGameOver()
{
    /* for (auto Pawn : TActorRange<APawn>(GetWorld()))
    {
        if (Pawn)
        {
            Pawn->TurnOff();
            Pawn->DisableInput(nullptr);
        }
    }*/
    SetGameState(EMLGameState::GameOver);
}

bool AMLGameModeBase::SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate)
{
    const auto PauseSet = Super::SetPause(PC, CanUnpauseDelegate);
    if (PauseSet)
    {
        SetGameState(EMLGameState::Pause);
    }
    return PauseSet;
}

bool AMLGameModeBase::OpenJournal(APlayerController* PC, FCanUnpause CanUnpauseDelegate)
{
    const auto PauseSet = Super::SetPause(PC, CanUnpauseDelegate);
    if (PauseSet)
    {
        SetGameState(EMLGameState::Journal);
    }
    return PauseSet;
}

void AMLGameModeBase::SetGameState(EMLGameState State)
{
    if (GameState == State)
        return;

    GameState = State;
    OnGameStateChanged.Broadcast(GameState);
}

bool AMLGameModeBase::ClearPause()
{
    const auto PauseCleared = Super::ClearPause();

    if (PauseCleared)
    {
        SetGameState(EMLGameState::InProgress);
    }
    
    return PauseCleared;
}
