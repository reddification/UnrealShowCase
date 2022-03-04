// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Wind.h"
#include "WindComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UWindComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UWindComponent();
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float TraceLenght = 100.0f;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float WindStrenght = 1.0f;

protected:
    // Called when the game starts
    virtual void BeginPlay() override;
    class AWind* WindOnScene = nullptr;

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere)
    bool DebugDraw = true;

private:
    bool HitObstacle = false;
};
