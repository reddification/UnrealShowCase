#pragma once

#include "CoreMinimal.h"
#include "MLBaseWidget.h"
#include "PlayerHUDWidget.h"
#include "UITypes.h"
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
    void ChangeGameState(EMLGameState State);
    void OpenJournal(APlayerController* PC);

    mutable FChangeMouseCursorEvent ChangeMouseCursorEvent;
    
protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UMLBaseWidget> PlayerHUDWidgetClass;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UMLBaseWidget> PauseWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UMLBaseWidget> GameOverWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UMLBaseWidget> JournalWidgetClass;

    virtual void BeginPlay() override;

private:
    UPROPERTY()
    TMap<EMLGameState, UMLBaseWidget*> GameWidgets;

    UPROPERTY()
    UMLBaseWidget* CurrentWidget = nullptr;

    void OnChangeMouseCursorEvent(bool bActive);
    void PrepareWidgets();
    bool bInitialized = false;
};
