#pragma once

#include "CoreMinimal.h"
#include "MLBaseWidget.h"
#include "Characters/Controllers/BasePlayerController.h"
#include "Game/MLCoreTypes.h"
#include "GameFramework/HUD.h"
#include "MLGameHUD.generated.h"

class UMLBaseWidget;

UCLASS()
class ALUMCOCKSGJ_API AMLGameHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;
    UPlayerHUDWidget* GetPlayerHUD();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> PlayerHUDWidgetClass;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> PauseWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> GameOverWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> JournalWidgetClass;

    virtual void BeginPlay() override;

private:
    UPROPERTY()
    TMap<EMLGameState, UMLBaseWidget*> GameWidgets;

    UPROPERTY()
    UMLBaseWidget* CurrentWidget = nullptr;

    void DrawCrossHair();
    void OnGameStateChanged(EMLGameState State);

    void PrepareWidgets();
    bool bInitialized = false;
};
