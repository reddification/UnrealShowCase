// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/SphereComponent.h"
#include "Tumbleweed.generated.h"

UCLASS()
class ALUMCOCKSGJ_API ATumbleweed : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATumbleweed();
    UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
    class USceneComponent*RootSceneComponent =nullptr;
    UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
    class UStaticMeshComponent*Mesh=nullptr;
    UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
    class USphereComponent*SphereTrigger=nullptr;
    UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
    float TumbleweedDamage = 10.0f;
    

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
    UFUNCTION()
    void OnCollidedCharacter(UPrimitiveComponent* OverlappedComponent, AActor*OtherActor, UPrimitiveComponent* OtherComponent,int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
