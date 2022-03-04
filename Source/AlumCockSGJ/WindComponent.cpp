// Fill out your copyright notice in the Description page of Project Settings.

#include "WindComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UWindComponent::UWindComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
    WindOnScene = Cast<AWind>(UGameplayStatics::GetActorOfClass(GetWorld(), AWind::StaticClass()));
    /* if (WindOnScene!=nullptr)
     {
         GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Red,TEXT("Wind Finded"));
     }
     */
}

// Called when the game starts
void UWindComponent::BeginPlay()
{
    Super::BeginPlay();
    HitObstacle = false;

    // ...
    WindOnScene = Cast<AWind>(UGameplayStatics::GetActorOfClass(GetWorld(), AWind::StaticClass()));
}

// Called every frame
void UWindComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!IsValid(WindOnScene))
        return;

    FVector WindLocation = WindOnScene->GetActorLocation();
    FRotator WindRotation = WindOnScene->GetActorRotation();
    FVector Start = GetOwner()->GetActorLocation();
    FVector End =
        Start - (WindRotation.Vector() * TraceLenght); //- FVector( UKismetMathLibrary::FindLookAtRotation(Start,WindLocation).Vector());

    FHitResult HitResult;
    FCollisionQueryParams TraceParams;

    GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel7, TraceParams);

    if (HitResult.GetActor() != nullptr)
    {
        HitObstacle = true;
    }
    if (HitResult.GetActor() == nullptr)
    {
        HitObstacle = false;
    }
    if (HitObstacle == false)
    {
        GetOwner()->SetActorLocation(Start + WindRotation.Vector() * WindStrenght);
        // GEngine->AddOnScreenDebugMessage(-1,0.1f,FColor::Red,TEXT("Startmoving?"));
    }
    if (DebugDraw)
    {
        DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.1f);
    }
}
