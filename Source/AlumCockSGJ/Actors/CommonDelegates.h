#pragma once

DECLARE_DELEGATE_OneParam(FAmmoChangedEvent, int32 ClipAmmo);
DECLARE_DELEGATE_TwoParams(FWeaponAmmoChangedEvent, int32 ClipAmmo, int32 RemainingAmmo);
DECLARE_MULTICAST_DELEGATE_TwoParams(FWeaponAmmoInfoChangedEvent, bool bInfiniteClip, bool bInfinityAmmoSupply);
DECLARE_DELEGATE_OneParam(FThrowablesCountChanged, int32 ThrowablesCount);
DECLARE_DELEGATE_TwoParams(FThrowableEquippedEvent, class AThrowableItem* Throwable, int32 Count);
DECLARE_MULTICAST_DELEGATE_OneParam(FInputLockedEvent, bool bLocked)
