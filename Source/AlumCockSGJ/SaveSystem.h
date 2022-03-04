#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveSystem.generated.h"

UCLASS()
class ALUMCOCKSGJ_API USaveSystem : public USaveGame
{
	GENERATED_BODY()
public:
    USaveSystem();
    UPROPERTY(EditAnywhere)
    FVector SaveLocation;
    TArray<FVector> ActorsLocations;
	void SaveGame();
    void LoadGame();
};
