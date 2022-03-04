// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AI/Data/DogTypes.h"
#include "PatrolPoint.generated.h"

UCLASS()
class ALUMCOCKSGJ_API APatrolPoint : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    APatrolPoint();
    UFUNCTION(BlueprintCallable)
    float GetDuration() { return Duration; };
    UFUNCTION(BlueprintCallable)
    EDogAction GetDogAction() { return DogAction; };
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EDogAction DogAction = EDogAction::Sniffs;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Duration = 3.0f;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
};
