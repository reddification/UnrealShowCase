#pragma once

#include "Data/EquipmentTypes.h"
#include "SaveSystem/SaveSubsystemInterface.h"
#include "Actors/Equipment/EquippableItem.h"
#include "EquipmentSettings.generated.h"

USTRUCT(BlueprintType)
struct FInitialLoadoutData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,SaveGame)
	TSubclassOf<AEquippableItem> EquippableItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,SaveGame)
	int InitialSupply = 30;
};

UCLASS()
class ALUMCOCKSGJ_API UEquipmentSettings : public UDataAsset, public ISaveSubsystemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Loadout",SaveGame)
	TArray<FInitialLoadoutData> InitialLoadout;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Loadout",SaveGame)
	TMap<EAmmunitionType, int32> AmmunitionLimits;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Loadout")
	EEquipmentSlot InitialEquippedWeapon = EEquipmentSlot::SideArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Loadout")
	EThrowableSlot InitialEquippedThrowable = EThrowableSlot::Explosive;
	
	// The smaller this value the faster changing equipment goes
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Loadout")
	float EquipDurationMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reload")
	bool bAutoReload = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reload")
	EReloadMode ReloadType = EReloadMode::DiscardUnspendAmmo;

	// The smaller this value the faster reloading goes
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reload")
	float ReloadDurationMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reload")
	FName ReloadMontageEndSectionName = FName("EndReload");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sockets")
	FName SecondaryHandSocket = FName("hand_l_socket");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAutoEquipNewItem = true;
};