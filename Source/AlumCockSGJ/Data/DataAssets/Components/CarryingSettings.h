#pragma once

#include "CoreMinimal.h"
#include "CommonConstants.h"
#include "CarryingSettings.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UCarryingSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Timings")
	float PickUpDurationSeconds = 0.5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Timings")
	float ReleaseDurationSeconds = 0.1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Timings")
	float PutDurationSeconds = 0.25;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CarryingMovementSpeed = 200.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Montages")
	UAnimMontage* PickUpMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Montages")
	UAnimMontage* ReleaseMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Montages")
	UAnimMontage* PutMontage;

	// technically it should be the item who specifies the socket
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName CharacterAttachmentSocket = SocketCarriedItemTwoHandedLight;
	
	// not utilized. just an idea
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxItemWeight = 0;
};
