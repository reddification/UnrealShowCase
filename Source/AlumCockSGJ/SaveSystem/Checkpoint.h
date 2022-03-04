// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGameSubsystem.h"
#include "SaveSubsystemInterface.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

UCLASS()
class ALUMCOCKSGJ_API ACheckpoint : public AActor, public ISaveSubsystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpoint();
    

    
    UFUNCTION(BlueprintCallable, Category = "Save Subsystem")
    void SaveGameAtCheckpoint();
    

   
    virtual void OnLevelDeserialized_Implementation() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
