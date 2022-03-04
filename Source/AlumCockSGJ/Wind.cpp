// Fill out your copyright notice in the Description page of Project Settings.

#include "Wind.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/ArrowComponent.h"

// Sets default values
AWind::AWind()
{

    SetRootComponent(RootArrow);

    RootArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RootArrow"));

    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    LineRotation = GetActorRotation();
    WindLocation = GetActorLocation();
    LoopWindCreation();
}

// Called when the game starts or when spawned
void AWind::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AWind::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // LoopWindCreation();
}
void AWind::CreateWindDirection()
{
    FVector Location;
    FRotator Rotation;
    FHitResult HitResult;

    Location = WindLocation;
    Rotation = LineRotation;

    FVector Start = Location; //- (Rotation.Vector()*0.5*WindLenght) ;
    FVector End = Start + (GetActorForwardVector() * WindLenght);
    FCollisionQueryParams TraceParams;

    GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel6, TraceParams);

    if (DebugDraw)
    {
        DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.1f);
    }
    if (HitResult.GetActor() != nullptr)
    {
        if (HitResult.GetComponent()->Mobility != EComponentMobility::Static)
        {
            HitResult.GetActor()->SetActorLocation(HitResult.GetActor()->GetActorLocation() + GetActorRotation().Vector() * WindStrenght);
        }
    }
}
void AWind::LoopWindCreation()
{
    for (float i = 0; i <= WindLenght; i = i + 30)
    {
        LineRotation = WindDirection;
        WindLocation = GetActorLocation();

        // float Angle = i*acosf(WindDirection.Roll);
        WindLocation = WindLocation + GetActorRightVector() * i; // FVector(WindLocation.X ,WindLocation.Y+i,WindLocation.Z);

        CreateWindDirection();
    }
}
