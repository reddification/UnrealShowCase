#pragma once

#include "Engine/DataTable.h"
#include "Sound/SoundCue.h"
#include "WorldItemDTR.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
    None = 0,
    MeeleeWeapon,
    RangeWeapon,
    Throwables,
    Readable,
    Consumable
};

USTRUCT(BlueprintType)
struct FWorldItemDTR : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Description;

    // UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    // Currently unused
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EItemType ItemType = EItemType::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class UStaticMesh* StaticMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class UTexture2D* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class USoundCue* PickupSFX;

    // this so fucking sucks omg
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<class ABaseWorldItem> SpawnWorldItemClass;
};