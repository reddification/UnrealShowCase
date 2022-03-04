// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGameSubsystem.h"
#include "SaveSubsystemTypes.h"
#include "Kismet/GameplayStatics.h"
#include "SaveSubsystemInterface.h"
#include "SaveSubsystemUtils.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "Serialization/ArchiveSaveCompressedProxy.h"
#include "Serialization/ArchiveLoadCompressedProxy.h"
#include "Engine/Level.h"
#include "Engine/LevelStreaming.h"

const FGameSaveData& USaveGameSubsystem::GetGameSaveData() const
{
	return GameSaveData;
}

void USaveGameSubsystem::SaveGame(FString SaveNameString)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::SaveGame(): %s"), *GetNameSafe(this));
    int32 SaveId = GetNextSaveId();
    FName SaveName (SaveNameString);
  
   
	SerializeGame();
	WriteSaveToFile(SaveName,SaveId);
  
}

void USaveGameSubsystem::QuickSave()
{
   /* UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::SaveGame(): %s"), *GetNameSafe(this));
     FName const SaveName (FName( FString("QuickSave")));
    int32 SaveId = 0;
    SerializeGame();
    WriteSaveToFile(SaveName,SaveId);*/
    FString SaveName("QuickSave");
    SaveGame(SaveName);
  
}

void USaveGameSubsystem::LoadLastGame()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadLastGame(): %s"), *GetNameSafe(this));

	if (SaveIds.Num() == 0)
	{
		UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadLastGame(): %s Failed! No saves."), *GetNameSafe(this));
		return;
	}

	LoadGame(SaveIds[SaveIds.Num() - 1]);
 
}

void USaveGameSubsystem::LoadGame(int32 SaveId)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadGame()"));
	if (!SaveIds.Contains(SaveId))
	{
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::LoadGame(): Failed!"));
		return;
	}
  
	LoadSaveFromFile(SaveId);
 
    UGameplayStatics::OpenLevel(this, GameSaveData.LevelName);
}

void USaveGameSubsystem::QuickLoad()
{
    UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadLastGame(): %s"), *GetNameSafe(this));

    if (SaveIds.Num() == 0)
    {
        UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadLastGame(): %s Failed! No saves."), *GetNameSafe(this));
        return;
    }

    LoadGame(SaveIds[0]);
}


void USaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::Initialize(): %s"), *GetNameSafe(this));

	GameSaveData = FGameSaveData();

	SaveDirectoryName = FString::Printf(TEXT("%sSaveGames/"), *FPaths::ProjectSavedDir());

	FSaveDirectoryVisitor DirectoryVisitor(SaveIds);
	FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*SaveDirectoryName, DirectoryVisitor);
	SaveIds.Sort();
    
    
    
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USaveGameSubsystem::OnPostLoadMapWithWorld);
}

void USaveGameSubsystem::Deinitialize()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::Deinitialize(): %s"), *GetNameSafe(this));

	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	Super::Deinitialize();
}

void USaveGameSubsystem::SerializeLevel(const ULevel* Level, const ULevelStreaming* StreamingLevel /*= nullptr*/)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::SerializeLevel(): %s, Level: %s, StreamingLevel: %s"), *GetNameSafe(this), *GetNameSafe(Level), *GetNameSafe(StreamingLevel));

	FLevelSaveData* LevelSaveData = nullptr;
	LevelSaveData = &GameSaveData.Level;

	TArray<FActorSaveData>& ActorsSaveData = LevelSaveData->ActorsSaveData;
	ActorsSaveData.Empty();

	for (AActor* Actor : Level->Actors)
	{
		if (!IsValid(Actor) || !Actor->Implements<USaveSubsystemInterface>())
		{
			continue;
		}

		FActorSaveData& ActorSaveData = ActorsSaveData[ActorsSaveData.AddUnique(FActorSaveData(Actor))];
		ActorSaveData.Transform = Actor->GetTransform();
		TArray<FObjectSaveData>& ComponentsSaveData = ActorSaveData.ComponentsSaveData;
		ComponentsSaveData.Empty();
		for (UActorComponent* ActorComponent : Actor->GetComponents())
		{
			if (ActorComponent->Implements<USaveSubsystemInterface>())
			{
				FObjectSaveData& ComponentSaveData = ComponentsSaveData[ComponentsSaveData.Emplace(ActorComponent)];
				FMemoryWriter MemoryWriter(ComponentSaveData.RawData, true);
				FSaveSubsystemArchive Archive(MemoryWriter, false);
				ActorComponent->Serialize(Archive);
			   
			}
		}

		FMemoryWriter MemoryWriter(ActorSaveData.RawData, true);
		FSaveSubsystemArchive Archive(MemoryWriter, false);
		Actor->Serialize(Archive);
	}
}

void USaveGameSubsystem::DeserializeLevel(ULevel* Level, const ULevelStreaming* StreamingLevel /*= nullptr*/)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s, Level: %s, StreamingLevel: %s"), *GetNameSafe(this), *GetNameSafe(Level), *GetNameSafe(StreamingLevel));

	FLevelSaveData* LevelSaveData = nullptr;
	LevelSaveData = &GameSaveData.Level;
	if (LevelSaveData == nullptr)
	{
		// There is no save for this level yet. Call OnLevelDeserialized() in all cases!
		USaveSubsystemUtil::BroadcastOnLevelDeserialized(Level);
		return;
	}

	TArray<AActor*> ActorsToNotify;

	TArray<FActorSaveData*> ActorsSaveData;
	ActorsSaveData.Reserve(LevelSaveData->ActorsSaveData.Num());
	for (FActorSaveData& ActorSaveData : LevelSaveData->ActorsSaveData)
	{
		ActorsSaveData.Add(&ActorSaveData);
	}

	for (TArray<AActor*>::TIterator ActorIterator = Level->Actors.CreateIterator(); ActorIterator; ++ActorIterator)
	{
		AActor* Actor = *ActorIterator;
		if (!IsValid(Actor) || !Actor->Implements<USaveSubsystemInterface>())
		{
			continue;
		}

		FActorSaveData* ActorSaveData = nullptr;
		for (TArray<FActorSaveData*>::TIterator ActorSaveDataIterator = ActorsSaveData.CreateIterator(); ActorSaveDataIterator; ++ActorSaveDataIterator)
		{
			if ((*ActorSaveDataIterator)->Name == Actor->GetFName())
			{
				ActorSaveData = *ActorSaveDataIterator;
				ActorSaveDataIterator.RemoveCurrent();
				break;
			}
		}

		if (ActorSaveData == nullptr)
		{
			UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s, ActorSaveData not found! Destroy actor: %s"), *GetNameSafe(this), *GetNameSafe(Actor));
			Actor->Destroy();
		}
		else
		{
			DeserializeActor(Actor, ActorSaveData);

			if (Actor->Implements<USaveSubsystemInterface>())
			{
				ActorsToNotify.Add(Actor);
			}
		}
	}

	UWorld* const World = GetWorld();

	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.OverrideLevel = Level;
	ActorSpawnParameters.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (FActorSaveData* ActorSaveData : ActorsSaveData)
	{
		UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s, Spawn new actor with name: %s"), *GetNameSafe(this), *ActorSaveData->Name.ToString());

		ActorSpawnParameters.Name = ActorSaveData->Name;

		// @bIgnoreOnActorSpawnedCallbackFast is hook to avoid double @OnLevelDeserialized invocation!
		BoolScopeWrapper OnActorSpawnedHook(bIgnoreOnActorSpawnedCallback, true);

		AActor* Actor = World->SpawnActor(ActorSaveData->Class.Get(), &ActorSaveData->Transform, ActorSpawnParameters);

		if (!IsValid(Actor))
		{
			// This is valid case. Actor can be IsPendingKill due some actor's logic started from overlaps.
			UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s, Failed to spawn new actor with name: %s"), *GetNameSafe(this), *ActorSaveData->Name.ToString());
			continue;
		}

		// Actor name can change so update it.
		ActorSaveData->Name = Actor->GetFName();

		DeserializeActor(Actor, ActorSaveData);

		if (Actor->Implements<USaveSubsystemInterface>())
		{
			ActorsToNotify.Add(Actor);
		}
	}

	for (AActor* Actor : ActorsToNotify)
	{
		NotifyActorsAndComponents(Actor);
	}
}
void USaveGameSubsystem::NotifyActorsAndComponents(AActor* Actor)
{
	ISaveSubsystemInterface::Execute_OnLevelDeserialized(Actor);
	for (UActorComponent* ActorComponent : Actor->GetComponents())
	{
		if (ActorComponent->Implements<USaveSubsystemInterface>())
		{
			ISaveSubsystemInterface::Execute_OnLevelDeserialized(ActorComponent);
		}
	}
}

void USaveGameSubsystem::SerializeGame()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::SerializeGame(): %s"), *GetNameSafe(this));

	UGameInstance* GameInstance = GetGameInstance();
	GameSaveData.GameInstance = FObjectSaveData(GetGameInstance());
	FMemoryWriter MemoryWriter(GameSaveData.GameInstance.RawData, true);
	FSaveSubsystemArchive Archive(MemoryWriter, false);
	GameInstance->Serialize(Archive);

	const UWorld* World = GetWorld();
	FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	if (World->IsPlayInEditor())
	{
		LevelName = UWorld::RemovePIEPrefix(LevelName);
	}

	GameSaveData.LevelName = FName(LevelName);

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (IsValid(PlayerCharacter))
	{
		GameSaveData.StartTransform = PlayerCharacter->GetTransform();
	}

	SerializeLevel(World->PersistentLevel);
}

void USaveGameSubsystem::DeserializeGame()
{    
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeGame(): %s"), *GetNameSafe(this));



      if (!GameSaveData.bIsSerialized)
      {
          return;
      }
      UGameInstance* GameInstance = GetGameInstance();
	FMemoryReader MemoryReader(GameSaveData.GameInstance.RawData, true);
	FSaveSubsystemArchive Archive(MemoryReader, false);
	GameInstance->Serialize(Archive);

	const UWorld* World = GetWorld();

	DeserializeLevel(World->PersistentLevel);
}

void USaveGameSubsystem::WriteSaveToFile(FName SaveName,int32 SaveId)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::WriteSaveToFile(): %s"), *GetNameSafe(this));

     
      GameSaveData.Name = SaveName;
   FString SaveNameString = SaveName.ToString();
      SaveIds.AddUnique(SaveId);
    SaveNames.AddUnique(SaveNameString);
	

	TArray<uint8> SaveBytes;
	FMemoryWriter MemoryWriter(SaveBytes);
	FObjectAndNameAsStringProxyArchive WriterArchive(MemoryWriter, false);
	GameSaveData.Serialize(WriterArchive);

	FArchive* FileWriter = IFileManager::Get().CreateFileWriter(*GetSaveFilePath(SaveId));

	if (bUseCompressedSaves)
	{
		TArray<uint8> CompressedSaveBytes;
		FArchiveSaveCompressedProxy CompressedArchive(CompressedSaveBytes, NAME_Zlib);
		CompressedArchive << SaveBytes;
		CompressedArchive.Flush();

		*FileWriter << CompressedSaveBytes;
	}
	else
	{
		*FileWriter << SaveBytes;
	}

	FileWriter->Close();
	delete FileWriter;
}

void USaveGameSubsystem::LoadSaveFromFile(int32 SaveId)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadSaveFromFile(): %s, SaveId %i"), *GetNameSafe(this), SaveId);

	FArchive* FileReader = IFileManager::Get().CreateFileReader(*GetSaveFilePath(SaveId));
	TArray<uint8> SaveBytes;

	if (bUseCompressedSaves)
	{
		TArray<uint8> CompressedSaveBytes;
		*FileReader << CompressedSaveBytes;

		FArchiveLoadCompressedProxy DecompressedArchive(CompressedSaveBytes, NAME_Zlib);
		DecompressedArchive << SaveBytes;
		DecompressedArchive.Flush();
	}
	else
	{
		*FileReader << SaveBytes;
	}

	FileReader->Close();
	delete FileReader;

	FMemoryReader MemoryReader(SaveBytes, true);
	FObjectAndNameAsStringProxyArchive ReaderArchive(MemoryReader, true);
	GameSaveData = FGameSaveData();
	GameSaveData.Serialize(ReaderArchive);
}

FString USaveGameSubsystem::GetSaveName(int32 SaveId)
{
   
    FArchive* FileReader = IFileManager::Get().CreateFileReader(*GetSaveFilePath(SaveId));
    TArray<uint8> SaveBytes;
    *FileReader << SaveBytes;
    FileReader->Close();
    delete FileReader;

    FMemoryReader MemoryReader(SaveBytes, true);
    FObjectAndNameAsStringProxyArchive ReaderArchive(MemoryReader, true);
    
   FBaseSaveData BaseSaveData = FBaseSaveData();
    BaseSaveData.Serialize(ReaderArchive);
    
    
        FString SaveName= BaseSaveData.Name.ToString();
        return SaveName;
}


void USaveGameSubsystem::OnPostLoadMapWithWorld(UWorld* LoadedWorld)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::OnPostLoadMapWithWorld(): %s, World: %s"), *GetNameSafe(this), *GetNameSafe(LoadedWorld));
	DeserializeGame();
}

void USaveGameSubsystem::DeserializeActor(AActor* Actor, const FActorSaveData* ActorSaveData)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeActor(): %s, Actor %s"), *GetNameSafe(this), *GetNameSafe(Actor));

	Actor->SetActorTransform(ActorSaveData->Transform);
	
	const TArray<FObjectSaveData>& ComponentsSaveData = ActorSaveData->ComponentsSaveData;
	for (UActorComponent* ActorComponent : Actor->GetComponents())
	{
		if (ActorComponent->Implements<USaveSubsystemInterface>())
		{
			const FObjectSaveData* ComponentSaveData = ComponentsSaveData.FindByPredicate([=](const FObjectSaveData& SaveData) { return SaveData.Name == ActorComponent->GetFName();} );
			FMemoryReader MemoryReader(ComponentSaveData->RawData, true);
			FSaveSubsystemArchive Archive(MemoryReader, false);
			ActorComponent->Serialize(Archive);
		}
	}

	FMemoryReader MemoryReader(ActorSaveData->RawData, true);
	FSaveSubsystemArchive Archive(MemoryReader, false);
	Actor->Serialize(Archive);
}

FString USaveGameSubsystem::GetSaveFilePath(int32 SaveId) const
{
	return SaveDirectoryName / FString::Printf(TEXT("%i.save"), SaveId);
}

int32 USaveGameSubsystem::GetNextSaveId() const
{
	if (SaveIds.Num() == 0)
	{
		return 1;
	}

	return SaveIds[SaveIds.Num()-1]+1;
}

void USaveGameSubsystem::OnActorSpawned(AActor* SpawnedActor)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::OnActorSpawned(): %s, Actor %s"), *GetNameSafe(this), *GetNameSafe(SpawnedActor));
	if (bIgnoreOnActorSpawnedCallback)
	{
		UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::OnActorSpawned(): %s, Skipped for actor %s. bIgnoreOnActorSpawnedCallback == true!"), *GetNameSafe(this), *GetNameSafe(SpawnedActor));
		return;
	}

	if (IsValid(SpawnedActor) && SpawnedActor->Implements<USaveSubsystemInterface>())
	{
		// We should notify a runtime spawned actors too.
		NotifyActorsAndComponents(SpawnedActor);
	}
}
