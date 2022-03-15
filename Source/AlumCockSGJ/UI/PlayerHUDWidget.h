#pragma once

#include "CoreMinimal.h"
#include "CharacterAttributesWidget.h"
#include "MLBaseWidget.h"
#include "Blueprint/UserWidget.h"
#include "Data/CharacterTypes.h"
#include "Data/UserInterfaceTypes.h"
#include "Data/Quests/QuestDialogue.h"
#include "Engine/DataTable.h"
#include "PlayerHUDWidget.generated.h"

class UDialogueWidget;

UCLASS(BlueprintType)
class ALUMCOCKSGJ_API UPlayerHUDWidget : public UMLBaseWidget
{
    GENERATED_BODY()

public:
    void OnAimingStateChanged(bool bAiming, EReticleType ReticleType);
    void OnPlayerSpeedChanged(float velocity);
    void OnPlayerShoot(float spread);
    void SetAmmo(int32 ClipAmmo, int32 RemainingAmmo);
    void SetThrowablesCount(int32 Count);
    void OnAttributeChanged(ECharacterAttribute Attribute, float Value);
    void SetWeaponName(const FText& Name);
    void SetThrowableName(const FText& Name);

    void SetReticleType(EReticleType ReticleType);
    void OnMeleeWeaponEquipped();
    void SetAmmoInfo(bool bInfiniteClip, bool bInfiniteAmmoSupply);

    bool TryOpenReadable(const FDataTableRowHandle& ReadableDTRH);
    void CloseReadable();
    void ReadableNextPage();
    void ReadablePreviousPage();
    void OnWeaponUnequipped();

	UFUNCTION(BlueprintImplementableEvent)
	void AddSubtitleLine(const FText& SpeakerName, const FText& SpeechText, float Duration);

	void StartDialogue();
	void EndDialogue();

	UDialogueWidget* GetDialogueWidget() const { return DialogueWidget; }
    void SetWeaponInfoVisible(bool bWeaponEquipped, bool bThrowablesEquipped);

protected:
	virtual void NativeConstruct() override;
	
    UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
    class UReticleWidget* Reticle;

    UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
    class UWeaponInfoWidget* WeaponInfoWidget;

    UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
    UCharacterAttributesWidget* CharacterAttributesWidget;

    UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
    class UReadableWidget* Readable;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UScrollBox* SubtitlesContainer;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UDialogueWidget* DialogueWidget;

    UPROPERTY(meta=(BindWidgetAnim), Transient)
    UWidgetAnimation* DamageAnimation;
};
