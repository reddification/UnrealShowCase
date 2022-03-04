// Fill out your copyright notice in the Description page of Project Settings.

#include "TumbleweedCreator.h"

#include "DrawDebugHelpers.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
ATumbleweedCreator::ATumbleweedCreator()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATumbleweedCreator::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ATumbleweedCreator::Tick(float DeltaTime)
{

    Super::Tick(DeltaTime);

    CreateSpawningBox();
}

void ATumbleweedCreator::SpawnTumbleweel()
{
    FActorSpawnParameters SpawnParameters;
    float ActorX = GetActorLocation().X;
    float ActorY = GetActorLocation().Y;

    float RandomX = FMath::RandRange(ActorX, RandomFloatX);
    float RandomY = FMath::RandRange(ActorY, RandomFloatY);
    FVector SpawnLocation = FVector(RandomX, RandomY, SpawnZ);
    FRotator SpawnRotation = FRotator(0, 0, 0);
    AActor* Spawn = GetWorld()->SpawnActor<AActor>(SpawningActor, SpawnLocation, SpawnRotation, SpawnParameters);
}
void ATumbleweedCreator::CreateSpawningBox()
{
    FVector ActorLocation = GetActorLocation();
    FVector Start1 = ActorLocation;
    if (DebugDraw)
    {

        FVector End1 = FVector(ActorLocation.X + SpawningCubeLenght, ActorLocation.Y, ActorLocation.Z);
        DrawDebugLine(GetWorld(), Start1, End1, FColor::Red, false, 5.0f);
        FVector Start2 = End1;
        FVector End2 = FVector(Start2.X, Start2.Y + SpawningCubeWide, Start2.Z);
        DrawDebugLine(GetWorld(), Start2, End2, FColor::Red, false, 5.0f);
        FVector Start3 = End2;
        FVector End3 = FVector(Start3.X - SpawningCubeLenght, Start3.Y, Start3.Z);
        DrawDebugLine(GetWorld(), Start3, End3, FColor::Red, false, 5.0f);
        FVector Start4 = End3;
        FVector End4 = FVector(Start4.X, Start4.Y - SpawningCubeWide, Start4.Z);
        DrawDebugLine(GetWorld(), Start4, End4, FColor::Red, false, 5.0f);
    }
    RandomFloatX = ActorLocation.X + SpawningCubeLenght;
    RandomFloatY = ActorLocation.Y + SpawningCubeWide;
}
