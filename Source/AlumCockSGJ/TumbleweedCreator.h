// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TumbleweedCreator.generated.h"

UCLASS()
class ALUMCOCKSGJ_API ATumbleweedCreator : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ATumbleweedCreator();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> SpawningActor;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpawningCubeLenght = 5000.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpawningCubeWide = 5000.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpawnZ;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool DebugDraw = true;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    UFUNCTION(BlueprintCallable)
    void SpawnTumbleweel();
    UFUNCTION(BlueprintCallable)
    void CreateSpawningBox();

private:
    float RandomFloatX;
    float RandomFloatY;
};
