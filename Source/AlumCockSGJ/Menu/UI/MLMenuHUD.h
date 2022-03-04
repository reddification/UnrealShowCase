#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MLMenuHUD.generated.h"

UCLASS()
class ALUMCOCKSGJ_API AMLMenuHUD : public AHUD
{
    GENERATED_BODY()
protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> MenuWidgetClass;

    virtual void BeginPlay() override;
};
