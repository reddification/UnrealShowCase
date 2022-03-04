#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Data/DataAssets/Components/CharacterVisionSettings.h"
#include "PlayerVisionComponent.generated.h"

struct FObservationParameters;
class IInteractable;
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UPlayerVisionComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    bool Interact() const;

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class UCharacterVisionSettings* CharacterVisionSettings;

private:
    TWeakObjectPtr<class APlayerCharacter> CharacterOwner;
    
    TWeakObjectPtr<AActor> ClosestInteractableActor;
    TWeakObjectPtr<AActor> ClosestInteractableUnderCursorActor;
    
    FTimerHandle UpdateNearSightTimer;

    void UpdateNearSight();
    TMap<AActor*, float> GetActorsInSight(const FVisionTraceSettings& TraceSettings);
    AActor* GetActorUnderCursor(const FVisionTraceSettings& TraceSettings);
    void GetObservationParameters(const FVector& eyesPosition, FRotator& eyesightRotation, const FVisionTraceSettings& TraceSettings,
        FObservationParameters& OutParameters) const;
};

struct FObservationParameters
{
    float VerticalAngleMax;
    float HorizontalAngleMax;
    FVector ViewCenter;
    float ActualSearchDistance;
};