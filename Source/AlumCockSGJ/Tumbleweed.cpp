// Fill out your copyright notice in the Description page of Project Settings.

#include "Tumbleweed.h"

#include "WindComponent.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Components/SphereComponent.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Components/Character/CharacterAttributesComponent.h"
#include "WindComponent.h"
#include "AI/Characters/AICharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ATumbleweed::ATumbleweed()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSphere"));
    SetRootComponent(RootSceneComponent);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    Mesh->SetupAttachment(RootSceneComponent);
    SphereTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("SphereTrigger"));
    SphereTrigger->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void ATumbleweed::BeginPlay()
{
    Super::BeginPlay();
    SphereTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATumbleweed::OnCollidedCharacter);
}

// Called every frame
void ATumbleweed::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
void ATumbleweed::OnCollidedCharacter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    // GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Red,TEXT("Triggered"));
    ABaseHumanoidCharacter* BaseCharacter = Cast<ABaseHumanoidCharacter>(OtherActor);

    if (OtherActor != nullptr)
    {
        if (OtherActor == BaseCharacter)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Characted Collided by tumbleweed"));
            BaseCharacter->TakeDamage(TumbleweedDamage, FDamageEvent(), NULL, OtherActor);
        }
        Destroy();
    }
}
