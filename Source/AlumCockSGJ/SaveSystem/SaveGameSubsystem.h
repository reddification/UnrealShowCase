// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveData.h"
#include "SaveGameSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ALUMCOCKSGJ_API USaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	public:
	const FGameSaveData& GetGameSaveData() const;
   

      UFUNCTION(BlueprintCallable, Category = "Save Subsystem")
    	void SaveGame(FString SaveNameString);

      UFUNCTION(BlueprintCallable, Category = "Save Subsystem")
      void QuickSave();
        
    	UFUNCTION(BlueprintCallable, Category = "Save Subsystem")
    	void LoadLastGame();
    
    	UFUNCTION(BlueprintCallable, Category = "Save Subsystem")
    	void LoadGame(int32 SaveId);

      UFUNCTION(BlueprintCallable, Category = "Save Subsystem")
      void QuickLoad();
      UFUNCTION(BlueprintCallable,Category = "Save Subsystem")
      FString GetSaveName(int32 SaveId);
    
    	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    	virtual void Deinitialize() override;
    
    	void SerializeLevel(const ULevel* Level, const ULevelStreaming* StreamingLevel = nullptr);
    	void DeserializeLevel(ULevel* Level, const ULevelStreaming* StreamingLevel = nullptr);

    UPROPERTY(BlueprintReadOnly, Category = "Save Subsystem")
    TArray<int32> SaveIds;

    UPROPERTY(BlueprintReadOnly, Category = "Save Subsystem")
    TArray<FString> SaveNames;
    
protected:
    void SerializeGame();
    void DeserializeGame();
    void WriteSaveToFile(FName SaveName,int32 SaveId);
    void LoadSaveFromFile(int32 SaveId);
    void OnPostLoadMapWithWorld(UWorld* LoadedWorld);
    void DeserializeActor(AActor* Actor, const FActorSaveData* ActorSaveData);
    FString GetSaveFilePath(int32 SaveId) const;
    int32 GetNextSaveId() const;
    void OnActorSpawned(AActor* SpawnedActor);
    void NotifyActorsAndComponents(AActor* Actor);


    FGameSaveData GameSaveData;
    FString SaveDirectoryName;
   
    FDelegateHandle OnActorSpawnedDelegateHandle;

    bool bUseCompressedSaves = false;
    /** Used to avoid double @OnLevelDeserialized invocation */
    bool bIgnoreOnActorSpawnedCallback = false;
};
