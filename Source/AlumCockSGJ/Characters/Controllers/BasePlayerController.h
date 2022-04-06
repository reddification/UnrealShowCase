#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"
#include "UI/PlayerHUDWidget.h"
#include "GCGameMode.h"
#include "Characters/PlayerCharacter.h"
#include "Game/MLCoreTypes.h"
#include "Data/PlayerGameStates.h"
#include "Data/Quests/QuestDTR.h"
#include "Data/Quests/QuestTaskDTR.h"
#include "Interfaces/KillConsiderate.h"
#include "UI/MLGameHUD.h"
#include "BasePlayerController.generated.h"

struct FDialogueParticipant;
class AInteractiveQuestActor;
class UObservationSourceComponent;
class ABaseHumanoidCharacter;
UCLASS()
class ALUMCOCKSGJ_API ABasePlayerController : public APlayerController, public IKillConsiderate
{
    GENERATED_BODY()

public:
    void OnItemPickedUp(const FDataTableRowHandle& ItemDTRH);
    void OnCharacterDied(const ABaseCharacter* BaseCharacter);
    virtual void SetPawn(APawn* InPawn) override;
    virtual void Tick(float DeltaSeconds) override;
    bool IsIgnoreCameraPitch() const { return bIgnoreCameraPitch; }
    void SetIgnoreCameraPitch(bool newValue) { bIgnoreCameraPitch = newValue; }
    APlayerCharacter* GetControlledCharacter() const { return PlayerCharacter; }
    virtual void BeginPlay() override;
    void ReadItem(const FDataTableRowHandle& ReadableDTRH);
    UPlayerHUDWidget* GetPlayerHUDWidget() const { return PlayerHUDWidget; }
    virtual void ReportKill(const AActor* KilledActor) override;
    void RegisterObservationSource(UObservationSourceComponent* ObservationSourceComponent);
    void ShowNpcSpeechSubtitles(const FText& NpcName, const FText& SpeechText, const FVector& NpcLocation, float SpeechLoudness) const;
    
    void OnDialogueStarted(bool bLocking, const FVector& NpcLocation);
    void OnDialogueEnded(const TArray<FDialogueParticipant>& Participants, bool bWasLocking, bool bCompleted = true);

protected:
    virtual void SetupInputComponent() override;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UPlayerHUDWidget* PlayerHUDWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag CutsceneGameplayTag;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float ObservationSourcesUpdateIntervalMilliseconds = 100.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MaxNpcSubtitlingRange = 1000.f;

    UPROPERTY()
    AMLGameHUD* GameHUD;

    void OnCutsceneStarted();
    void OnCutsceneEnded();

private:
    UPROPERTY()
    APlayerCharacter* PlayerCharacter;

    UPROPERTY()
    TArray<UObservationSourceComponent*> ObservationSources;
    
    void UpdateObservationSources();
    FTimerHandle ObservationSourcesUpdateTimer;
    
    void StartSliding();
    void StopSliding();
    void StartWallrun();
    void StopWallrun();
    void Interact();
    void Climb(float Value);
    void SwimUp(float Value);
    void SwimRight(float Value);
    void SwimForward(float Value);

    void TurnAtRate(float Value);
    void LookUpAtRate(float Value);
    void LookUp(float Value);
    void Turn(float Value);
    void MoveForward(float Value);
    void MoveRight(float Value);

    void Mantle();
    void Jump();
    void ToggleCrouchState();
    void ToggleProneState();

    void StartSprint();
    void StopSprint();

    void StopFiring();
    void StartFiring();
    void StartAiming();
    void StopAiming();
    void Reload();

    void PickNextWeapon();
    void PickPreviousWeapon();

    void ThrowItem();

    void ToggleFireMode();

    void StartPrimaryMeleeAttack();
    void StopPrimaryMeleeAttack();
    void StartSecondaryMeleeAttack();
    void StopSecondaryMeleeAttack();
    
    void SkipCutscene();
    
    void CloseReadable();
    void ShowNextPage();
    void ShowPreviousPage();
    
    void SkipDialogueLine();
    
    void UnequipWeapon();
    void EquipPrimaryWeapon();
    void EquipSecondaryWeapon();
    void EquipMeleeWeapon();
    void EquipPrimaryThrowable();
    void EquipSecondaryThrowable();

    void ToggleWalking();

    void OnAttributeChanged(ECharacterAttribute Attribute, float Percent) const;

    bool bIgnoreCameraPitch;
    void OnAimingStateChanged(bool bAiming, class ARangeWeaponItem* Weapon);
    void OnAmmoChanged(int32 NewAmmo, int32 TotalAmmo);
    void OnThrowableEquipped(class AThrowableItem* Throwable, int32 Count);
    void OnThowablesCountChanged(int32 Count);

    void OnWeaponEquipped(const FText& Name, EReticleType Reticle);
    void OnWeaponUnequipped();
    void UpdateWeaponInfoVisibility();
    void OnMeleeWeaponEquipped();
    void OnChangeMouseCursorState(bool bShowCursor);
    void OnPauseGame();
    void OpenJournal();
    void SaveGame();
    void LoadGame();

    void OnWeaponAmmoInfoChanged(bool bInfiniteClip, bool bInfiniteAmmoSupply);
    
    void OnQuestStarted(const FQuestDTR* QuestDTR);
    void OnQuestEventOccured(const FQuestTaskDTR* QuestEventDTRH);
    void OnQuestCompleted(const FQuestDTR* QuestDTR, bool bAutocompleted);

    void SetInputLocked(bool bInputLocked);
    bool bCharacterInputEnabled = true;
    
    EPlayerGameState PlayerGameState = EPlayerGameState::Game;
};
