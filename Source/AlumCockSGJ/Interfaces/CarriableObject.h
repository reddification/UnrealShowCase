#pragma once

#include "CoreMinimal.h"
#include "CommonConstants.h"
#include "UObject/Interface.h"
#include "CarriableObject.generated.h"

UENUM(BlueprintType)
enum class ECarryingType : uint8
{
	None,
	OneHanded,
	TwoHandedLight,
	TwoHandedHeavy
};

UINTERFACE()
class UCarriableObject : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API ICarriableObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void OnCarriedStart();
	virtual void OnCarriedStart_Implementation() {}

	UFUNCTION(BlueprintNativeEvent)
	void OnCarriedStop();
	virtual void OnCarriedStop_Implementation() {}

	UFUNCTION(BlueprintNativeEvent)
	ECarryingType GetCarryingObjectType();
	virtual ECarryingType GetCarryingObjectType_Implementation() { return ECarryingType::TwoHandedLight; }

	UFUNCTION(BlueprintNativeEvent)
	FName GetCharacterAttachmentSocket();
	virtual FName GetCharacterAttachmentSocket_Implementation() { return SocketCarriedItemTwoHandedLight; }
};
