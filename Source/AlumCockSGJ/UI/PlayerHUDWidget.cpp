#include "UI/PlayerHUDWidget.h"

#include "DialogueWidget.h"
#include "ReadableWidget.h"
#include "WeaponInfoWidget.h"
#include "ReticleWidget.h"
#include "Components/TextBlock.h"

void UPlayerHUDWidget::SetAmmoInfoVisible(bool bVisible)
{
    WeaponInfoWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPlayerHUDWidget::OnAimingStateChanged(bool bAiming, EReticleType ReticleType)
{
    Reticle->OnAimingStateChanged(bAiming);
    Reticle->SetReticleType(ReticleType);
}

void UPlayerHUDWidget::OnPlayerSpeedChanged(float velocity)
{
    Reticle->OnPlayerSpeedChanged(velocity);
}

void UPlayerHUDWidget::OnPlayerShoot(float spread)
{
    Reticle->OnPlayerShoot(spread);
}

void UPlayerHUDWidget::SetAmmo(int32 ClipAmmo, int32 RemainingAmmo)
{
    WeaponInfoWidget->SetAmmo(ClipAmmo, RemainingAmmo);
}

void UPlayerHUDWidget::SetThrowablesCount(int32 Count)
{
    WeaponInfoWidget->SetThrowablesCount(Count);
}

void UPlayerHUDWidget::OnAttributeChanged(ECharacterAttribute Attribute, float Value)
{
    CharacterAttributesWidget->SetAttribute(Attribute, Value);
    if (!IsAnimationPlaying(DamageAnimation) && Attribute == ECharacterAttribute::Health)
    {
        PlayAnimation(DamageAnimation);
    }
}

void UPlayerHUDWidget::SetReticleType(EReticleType ReticleType)
{
    Reticle->SetReticleType(ReticleType);
}

void UPlayerHUDWidget::OnMeleeWeaponEquipped()
{
    Reticle->SetReticleType(EReticleType::None);
    WeaponInfoWidget->OnMeleeWeaponEquipped();
}

void UPlayerHUDWidget::SetAmmoInfo(bool bInfiniteClip, bool bInfiniteAmmoSupply)
{
    WeaponInfoWidget->SetWeaponInfo(bInfiniteClip, bInfiniteAmmoSupply);
}

bool UPlayerHUDWidget::TryOpenReadable(const FDataTableRowHandle& ReadableDTRH)
{
    bool bOpened = Readable->TryOpenReadable(ReadableDTRH);
    if (bOpened)
        Readable->SetVisibility(ESlateVisibility::HitTestInvisible);

    return bOpened;
}

void UPlayerHUDWidget::CloseReadable()
{
    Readable->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerHUDWidget::ReadableNextPage()
{
    Readable->NextPage();
}

void UPlayerHUDWidget::ReadablePreviousPage()
{
    Readable->PreviousPage();
}

void UPlayerHUDWidget::OnWeaponUnequipped()
{
    SetReticleType(EReticleType::None);
}

void UPlayerHUDWidget::StartDialogue()
{
	// bool bDialogueStarted = DialogueWidget->StartDialogue(NewDialogue);
	// if (bDialogueStarted)
	// 	DialogueWidget->SetVisibility(ESlateVisibility::Visible);
	//
	// return bDialogueStarted;
	DialogueWidget->SetVisibility(ESlateVisibility::Visible);
}

void UPlayerHUDWidget::EndDialogue()
{
	DialogueWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerHUDWidget::SetWeaponName(const FText& Name)
{
    WeaponInfoWidget->ActiveWeaponNameTextblock->SetText(Name);
}

void UPlayerHUDWidget::SetThrowableName(const FText& Name)
{
    WeaponInfoWidget->ActiveThrowableNameTextblock->SetText(Name);
}
