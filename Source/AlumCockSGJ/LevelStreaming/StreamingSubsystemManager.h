#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelStreaming.h"
#include "UObject/Object.h"
#include "StreamingSubsystemManager.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UStreamingSubsystemManager : public UObject
{
	GENERATED_BODY()

private:
	FString LevelName;
	ULevelStreaming::ECurrentState StreamingLevelState;
	TWeakObjectPtr<ULevelStreaming> StreamingLevel;
};
