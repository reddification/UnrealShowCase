#include "UI/WeaponInfoWidget.h"

#include "Components/TextBlock.h"

void UWeaponInfoWidget::SetAmmo(int32 CurrentAmmo, int32 TotalAmmo)
{
	if (!bInfiniteClip)
		ClipAmmoTextblock->SetText(FText::AsNumber(CurrentAmmo));
	if (!bInfiniteAmmoSupply)
		RemainingAmmoTextblock->SetText(FText::AsNumber(TotalAmmo));

	if (AmmoWidgetsContainerWidget->Visibility == ESlateVisibility::Hidden)
	{
		AmmoWidgetsContainerWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UWeaponInfoWidget::UpdateVisiblity(bool bWeaponEquipped, bool bThrowableEquipped)
{
	SetVisibility(bWeaponEquipped || bThrowableEquipped ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	SetWeaponBlockVisible(bWeaponEquipped);
	SetThrowableBlockVisible(bThrowableEquipped);
}

void UWeaponInfoWidget::SetThrowablesCount(int32 ThrowablesCount)
{
	RemainingThrowablesTextblock->SetText(ThrowablesCount > 0 ? FText::AsNumber(ThrowablesCount) : FText::GetEmpty());
}

void UWeaponInfoWidget::OnMeleeWeaponEquipped()
{
	AmmoWidgetsContainerWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UWeaponInfoWidget::SetWeaponInfo(bool bNewInfiniteClip, bool bNewInfiniteAmmoSupply)
{
	bInfiniteClip = bNewInfiniteClip;
	bInfiniteAmmoSupply = bNewInfiniteAmmoSupply;
	if (bNewInfiniteClip)
		ClipAmmoTextblock->SetText(FText::FromString(UTF8_TO_TCHAR("∞")));

	if (bNewInfiniteAmmoSupply)
		RemainingAmmoTextblock->SetText(FText::FromString(UTF8_TO_TCHAR("∞")));
}

void UWeaponInfoWidget::SetWeaponBlockVisible(bool bVisible)
{
	WeaponBlock->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UWeaponInfoWidget::SetThrowableBlockVisible(bool bEquipped)
{
	ThrowablesBlock->SetVisibility(bEquipped ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UWeaponInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ClipAmmoTextblock->SetText(FText::GetEmpty());
	RemainingAmmoTextblock->SetText(FText::GetEmpty());
	ActiveWeaponNameTextblock->SetText(FText::GetEmpty());
	ActiveThrowableNameTextblock->SetText(FText::GetEmpty());
	RemainingThrowablesTextblock->SetText(FText::GetEmpty());

}
