// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SoundComponent.h"
#include "Components/Data/SoundDTR.h"

// Sets default values for this component's properties
USoundComponent::USoundComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}

// Called when the game starts
void USoundComponent::BeginPlay()
{
    Super::BeginPlay();
    InitializeComponent();
}

void USoundComponent::InitializeComponent()
{
    if (SoundDTRH.IsNull())
        return;

    FSoundDTR* SoundDTR = SoundDTRH.DataTable->FindRow<FSoundDTR>(SoundDTRH.RowName, "");
    if (!SoundDTR)
        return;
}

void USoundComponent::PlaySound()
{

    FSoundDTR* SoundDTR = SoundDTRH.DataTable->FindRow<FSoundDTR>(SoundDTRH.RowName, "");
    if (!SoundDTR)
        return;

    if (!SoundDTR->FmodSoundEvent)
    {
        UFMODBlueprintStatics::EventInstanceSetParameter(InstanceWrapper, FName("object_type"), SoundDTR->ObjectType);
        InstanceWrapper = UFMODBlueprintStatics::PlayEvent2D(GetWorld(), SoundDTR->FmodSoundEvent, true);

        if (!GetWorld()->GetTimerManager().IsTimerActive(EventTimer))
        {
            GetWorld()->GetTimerManager().SetTimer(
                EventTimer, this, &USoundComponent::StopSound, SoundDTR->Duration, false);
        }
    }
}

void USoundComponent::StopSound()
{
    InstanceWrapper.Instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
    InstanceWrapper.Instance->release();
}

// Called every frame
void USoundComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}
