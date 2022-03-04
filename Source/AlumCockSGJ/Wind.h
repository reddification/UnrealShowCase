// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "GameFramework/Actor.h"
#include "Wind.generated.h"

UCLASS()
class ALUMCOCKSGJ_API AWind : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AWind();
    UPROPERTY(BlueprintReadWrite)
    float WindStrenght = 1.0f;
    FVector WindLocation;

    UPROPERTY(BlueprintReadWrite)
    float WindLenght = 5000.0f;
    UPROPERTY(BlueprintReadWrite)
    FRotator WindDirection;
    UPROPERTY(BlueprintReadWrite)
    bool DebugDraw = true;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    UPROPERTY(BlueprintReadWrite)
    class USceneComponent* RootArrow = nullptr;

    void CreateWindDirection();
    void LoopWindCreation();

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    FRotator LineRotation;

private:
};
