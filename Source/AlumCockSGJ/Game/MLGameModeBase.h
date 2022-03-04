// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Game/MLCoreTypes.h"
#include "MLGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ALUMCOCKSGJ_API AMLGameModeBase : public AGameMode
{
    GENERATED_BODY()
public:
    FOnGameStateChangedSignature OnGameStateChanged;
    void OnGameOver();
    virtual bool ClearPause() override;
    void SetGameState(EMLGameState State);

    virtual bool SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate = FCanUnpause()) override;
    bool OpenJournal(APlayerController* PC, FCanUnpause CanUnpauseDelegate = FCanUnpause());

protected:
    virtual void StartPlay() override;

private:
    EMLGameState GameState = EMLGameState::WaitingToStart;
};
