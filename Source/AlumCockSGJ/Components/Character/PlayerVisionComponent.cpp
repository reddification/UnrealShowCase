#include "PlayerVisionComponent.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Data/DataAssets/Components/CharacterVisionSettings.h"
#include "Interfaces/Interactable.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/TraceUtils.h"
#include "Characters/Controllers/BasePlayerController.h"

typedef UKismetMathLibrary KML;
typedef UKismetSystemLibrary KSL;

void UPlayerVisionComponent::BeginPlay()
{
    Super::BeginPlay();
    checkf(IsValid(CharacterVisionSettings), TEXT("CharacterVisionSettings DataAsset must be set"));
    CharacterOwner = Cast<APlayerCharacter>(GetOwner());
    checkf(CharacterOwner.IsValid(), TEXT("Character must be APlayerCharacter"));
    GetWorld()->GetTimerManager().SetTimer(UpdateNearSightTimer, this, &UPlayerVisionComponent::UpdateNearSight,
        CharacterVisionSettings->NearTraceSettings.SightIntervalMilliseconds / 1000.f, true);
}

bool UPlayerVisionComponent::Interact() const
{
    if (ClosestInteractableActor.IsValid())
    {
        auto Interactable = Cast<IInteractable>(ClosestInteractableActor.Get());
        return Interactable->InteractWithPlayer(CharacterOwner.Get());
    }

    if (ClosestInteractableUnderCursorActor.IsValid())
    {
        auto Interactable = Cast<IInteractable>(ClosestInteractableUnderCursorActor.Get());
        return Interactable->InteractWithPlayer(CharacterOwner.Get());
    }

    return false;
}

void UPlayerVisionComponent::UpdateNearSight()
{
    auto TracedActors = GetActorsInSight(CharacterVisionSettings->NearTraceSettings);
    float MinDeltaDistance = FLT_MAX;
    AActor* NewClosestInteractableActor = nullptr;

    for (const auto& TracedActorInfo : TracedActors)
    {
        if (TracedActorInfo.Value < MinDeltaDistance)
        {
            NewClosestInteractableActor = TracedActorInfo.Key;
            MinDeltaDistance = TracedActorInfo.Value;
        }
    }

    if (ClosestInteractableActor != NewClosestInteractableActor)
    {
        if (ClosestInteractableActor.IsValid())
        {
            auto OldInteractable = Cast<IInteractable>(ClosestInteractableActor.Get());
            OldInteractable->OutOfSight();
        }

        if (IsValid(NewClosestInteractableActor))
        {
            auto NewInteractable = Cast<IInteractable>(NewClosestInteractableActor);
            NewInteractable->InSight();
        }

        ClosestInteractableActor = NewClosestInteractableActor;
    }

    AActor* NewClosestInteractableUnderCursorActor = GetActorUnderCursor(CharacterVisionSettings->NearTraceSettings);
    if (ClosestInteractableUnderCursorActor != NewClosestInteractableUnderCursorActor)
    {
        if (ClosestInteractableUnderCursorActor.IsValid())
        {
            auto OldInteractable = Cast<IInteractable>(ClosestInteractableUnderCursorActor.Get());
            OldInteractable->OutOfSightUnderCursor();
        }

        if (IsValid(NewClosestInteractableUnderCursorActor))
        {
            auto NewInteractable = Cast<IInteractable>(NewClosestInteractableUnderCursorActor);
            NewInteractable->InSightUnderCursor();
        }

        ClosestInteractableUnderCursorActor = NewClosestInteractableUnderCursorActor;
    }

}

AActor* UPlayerVisionComponent::GetActorUnderCursor(const FVisionTraceSettings& TraceSettings)
{
    auto PlayerController = Cast<ABasePlayerController>(CharacterOwner->GetOwner());
    if (!IsValid(PlayerController))
    {
        return nullptr;
    }

    auto PlayerCameraManager = PlayerController->PlayerCameraManager;
    if (!IsValid(PlayerCameraManager))
    {
        return nullptr;
    }

    FVector Start = PlayerCameraManager->GetCameraLocation();
    FVector End = Start + PlayerCameraManager->GetActorForwardVector() * TraceSettings.InteractionDistanceUnderCursor;
    auto TraceDebugMode = TraceSettings.DebugTraceUnderCursor;

    FHitResult HitResult;
    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(GetOwner());
    bool hit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, queryParams);

    if (TraceDebugMode == EDebugTraceUnderCursorMode::Visible)
    {
        TraceUtils::DrawDebugLineTrace(GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, EDrawDebugTrace::ForDuration, hit, HitResult,
            FLinearColor::Red, FLinearColor::Green, 1, false);
    }

    if (hit)
    {
        AActor* Actor = HitResult.GetActor();
        if (Actor == nullptr) // BSP
            return nullptr;

        bool bActorIsInteractable = Actor->Implements<UInteractable>();
        if (!bActorIsInteractable)
            return nullptr;

        return Actor;
    }

    return nullptr;
}

TMap<AActor*, float> UPlayerVisionComponent::GetActorsInSight(const FVisionTraceSettings& TraceSettings)
{
    auto world = GetWorld();
    TMap<AActor*, float> CurrentlyTracedActors;
    FObservationParameters parameters;

    FVector eyesPosition = GetComponentLocation();
    FRotator eyesightRotation = CharacterOwner->GetViewRotation();
    auto TraceDebugMode = TraceSettings.DebugTraceVerbocity;
    GetObservationParameters(eyesPosition, eyesightRotation, TraceSettings, parameters);
    const float DeltaVertAngle = FMath::Lerp(parameters.VerticalAngleMax, 0.f, TraceSettings.TraceDensity);
    const float DeltaHorizAngle = FMath::Lerp(parameters.HorizontalAngleMax, 0.f, TraceSettings.TraceDensity); // * 16/9;

    FHitResult HitResult;
    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(GetOwner());

    for (float currentVerticalAngle = -parameters.VerticalAngleMax / 2; currentVerticalAngle < parameters.VerticalAngleMax / 2;
         currentVerticalAngle += DeltaVertAngle)
    {
        for (float currentHorizAngle = -parameters.HorizontalAngleMax / 2; currentHorizAngle < parameters.HorizontalAngleMax / 2;
             currentHorizAngle += DeltaHorizAngle)
        {
            FRotator Rotator = FRotator(
                FMath::Clamp(eyesightRotation.Pitch + currentVerticalAngle, -90.f, 90.f), eyesightRotation.Yaw + currentHorizAngle, 0);
            FVector tracePoint = eyesPosition + (KML::GetForwardVector(Rotator) * parameters.ActualSearchDistance);
            bool hit = world->LineTraceSingleByChannel(HitResult, eyesPosition, tracePoint, ECC_Visibility, queryParams);
            if (TraceDebugMode >= EDebugMode::UltraVerbose)
            {
                TraceUtils::DrawDebugLineTrace(world, HitResult.TraceStart, HitResult.TraceEnd, EDrawDebugTrace::ForDuration, hit,
                    HitResult, FLinearColor::Red, FLinearColor::Green, 1, false);
            }

            if (hit)
            {
                AActor* Actor = HitResult.GetActor();
                if (Actor == nullptr) // BSP
                    continue;

                float* NearestSquaredDistance = CurrentlyTracedActors.Find(Actor);
                bool bActorIsInteractable = Actor->Implements<UInteractable>();
                if (!bActorIsInteractable)
                    continue;

                if (TraceDebugMode >= EDebugMode::Verbose)
                {
                    TraceUtils::DrawDebugLineTrace(world, HitResult.TraceStart, HitResult.ImpactPoint, EDrawDebugTrace::ForDuration, hit,
                        HitResult, FLinearColor::Yellow, FLinearColor::Blue, 1, false);
                }

                float ActorSquaredDistance = FVector::DistSquared(parameters.ViewCenter, HitResult.ImpactPoint);
                if (NearestSquaredDistance == nullptr || ActorSquaredDistance < *NearestSquaredDistance)
                    CurrentlyTracedActors.Add(Actor, ActorSquaredDistance);
            }
        }
    }

    return CurrentlyTracedActors;
}

void UPlayerVisionComponent::GetObservationParameters(const FVector& eyesPosition, FRotator& eyesightRotation,
    const FVisionTraceSettings& TraceSettings, FObservationParameters& OutParameters) const
{
    FVector ViewDirection = eyesightRotation.Vector();
    eyesightRotation.Pitch = eyesightRotation.Pitch > 180 ? eyesightRotation.Pitch - 360 : eyesightRotation.Pitch;
    float Alpha = FMath::Abs(eyesightRotation.Pitch) / TraceSettings.MaxSearchAngle;
    float actualSearchAngle = KML::Lerp(TraceSettings.MinSearchAngle, TraceSettings.MaxSearchAngle, Alpha);

    OutParameters.ActualSearchDistance = FMath::Lerp(TraceSettings.MinInteractionDistance, TraceSettings.MaxInteractionDistance, Alpha);
    OutParameters.VerticalAngleMax = actualSearchAngle;
    OutParameters.HorizontalAngleMax = actualSearchAngle * 16 / 9;
    OutParameters.ViewCenter = eyesPosition + (ViewDirection * OutParameters.ActualSearchDistance);
}
