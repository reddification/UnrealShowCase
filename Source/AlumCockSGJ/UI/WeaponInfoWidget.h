#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponInfoWidget.generated.h"

class UTextBlock;

UCLASS()
class ALUMCOCKSGJ_API UWeaponInfoWidget : public UUserWidget
{
	GENERATED_BODY()

friend class UPlayerHUDWidget;
	
public:
	void SetAmmo(int32 CurrentAmmo, int32 TotalAmmo);
	void UpdateVisiblity(bool bWeaponEquipped, bool bThrowableEquipped);
	void SetThrowablesCount(int32 ThrowablesCount);
	void OnMeleeWeaponEquipped();
	void SetWeaponInfo(bool bNewInfiniteClip, bool bNewInfiniteAmmoSupply);
	void SetWeaponBlockVisible(bool bVisible);
	void SetThrowableBlockVisible(bool bThrowablesEquipped);

protected:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* ActiveWeaponNameTextblock;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* ActiveThrowableNameTextblock;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UWidget* AmmoWidgetsContainerWidget;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UWidget* WeaponBlock;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UWidget* ThrowablesBlock;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* ClipAmmoTextblock;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* RemainingAmmoTextblock;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* RemainingThrowablesTextblock;

	virtual void NativeConstruct() override;
	
private:
	bool bInfiniteClip = false;
	bool bInfiniteAmmoSupply = false;
};
