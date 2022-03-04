#pragma once
#include "Data/DebugEnums.h"

#include "CharacterVisionSettings.generated.h"

USTRUCT(BlueprintType)
struct FVisionTraceSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float InteractionDistanceUnderCursor = 400.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EDebugTraceUnderCursorMode DebugTraceUnderCursor = EDebugTraceUnderCursorMode::None;

    /** Interactable actor search distance when the character looks forward (camera in/declination) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float MinInteractionDistance = 100.f;

    /** Interactable actor search distance when the character looks down (camera in/declination) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float MaxInteractionDistance = 230.f;

    /** Search angle when the character looks forward (camera in/declination) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float MinSearchAngle = 45.f;

    /** Search angle when the character looks down (camera in/declination) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float MaxSearchAngle = 90.f;

    /** Long range observation distance */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float ObservationDistance = 3000.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float SightIntervalMilliseconds = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (UIMin = 0.1, ClampMin = 0.1, UIMax = 0.95f, ClampMax = 0.95f))
    float TraceDensity = 0.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
    EDebugMode DebugTraceVerbocity = EDebugMode::Brief;
};

UCLASS()
class ALUMCOCKSGJ_API UCharacterVisionSettings : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVisionTraceSettings NearTraceSettings;
};
