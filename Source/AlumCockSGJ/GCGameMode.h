// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "GCGameMode.generated.h"

UENUM(BlueprintType)
enum class EGCMatchStatus : uint8
{
    WaitingToStart = 0,
    InProgress,
    Pause,
    GameOver
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMatchStatusChangedSignature, EGCMatchStatus);

UCLASS()
class ALUMCOCKSGJ_API AGCGameMode : public AGameMode
{
    GENERATED_BODY()
public:
    FOnMatchStatusChangedSignature OnMatchStatusChanged;
    virtual bool SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate) override;
    void OnGameOver();
    virtual bool ClearPause() override;
    void SetMatchStatus(EGCMatchStatus Status);

protected:
    virtual void StartPlay() override;

private:
    EGCMatchStatus MatchStatus = EGCMatchStatus::WaitingToStart;
};
