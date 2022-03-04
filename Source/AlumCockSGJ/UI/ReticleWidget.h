#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/UserInterfaceTypes.h"
#include "ReticleWidget.generated.h"

class UCanvasPanel;

UCLASS()
class ALUMCOCKSGJ_API UReticleWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent)
    void OnAimingStateChanged(bool bAiming);

    UFUNCTION(BlueprintImplementableEvent)
    void OnPlayerSpeedChanged(float playerSpeed);

    UFUNCTION(BlueprintImplementableEvent)
    void OnPlayerShoot(float maxSpread);

    void SetReticleType(EReticleType NewReticleType);

    UPROPERTY()
    EReticleType ReticleType;

    UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
    UCanvasPanel* CrosshairCanvas;

    UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
    UCanvasPanel* ScopeCanvas;
};
