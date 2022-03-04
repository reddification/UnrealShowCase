// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Engine/DataTable.h"
#include "FMODBlueprintStatics.h"
#include "SoundComponent.generated.h"

UCLASS(ClassGroup = (Audio), meta = (BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API USoundComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    USoundComponent();

    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Components")
    void PlaySound();
    void StopSound();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FDataTableRowHandle SoundDTRH;

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    void InitializeComponent();

    FFMODEventInstance InstanceWrapper;
    FTimerHandle EventTimer;
    
};
