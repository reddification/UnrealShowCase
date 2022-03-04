// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSystem/Checkpoint.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
  

}

void ACheckpoint::SaveGameAtCheckpoint()
{
    USaveGameSubsystem* SaveGameSubsystem  = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<USaveGameSubsystem>();
    Destroy();
    if (SaveGameSubsystem!= nullptr)
    {
        SaveGameSubsystem->QuickSave();
       
        
        
    }
    
}

void ACheckpoint::OnLevelDeserialized_Implementation()
{
   
}


// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

