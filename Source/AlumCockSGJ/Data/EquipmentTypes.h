#pragma once

UENUM(BlueprintType)
enum class EAmmunitionType : uint8
{
	None = 0,
	Pistol = 1,
	AssaultRifle,
	Shotgun,
	SniperRifle,
	Dynamites,
	RifleGrenades,
	MAX  UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EReloadMode : uint8
{
	None = 0 UMETA(Hidden),
	KeepUnspentAmmo = 1,
	DiscardUnspendAmmo = 2
};

// Order of enumeration matters
UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	None = 0,
	MeleeWeapon,
	SideArm,
	PrimaryWeapon,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EReloadType : uint8
{
	None = 0,
	FullClip = 1,
	ShellByShell = 2
};

UENUM(BlueprintType)
enum class EEquippableItemType : uint8
{
	None,
	OneHandedRanged = 1,
	TwoHandedRanged,
	Throwable,
	Melee
};

UENUM(BlueprintType)
enum class EThrowableSlot : uint8
{
	None = 0,
	Explosive = 1,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponFireMode : uint8
{
	Single,
	Burst,
	FullAuto,
};

UENUM(BlueprintType)
enum class EMeleeAttackType : uint8
{
	None = 0,
	Primary,
	Secondary,
	MAX UMETA(Hidden)
};

