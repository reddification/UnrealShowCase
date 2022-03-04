#pragma once

#include "CoreMinimal.h"
#include "Characters/Animations/Notifies/AnimNotify_CarryingStateChanged.h"
#include "Components/ActorComponent.h"
#include "Interfaces/CarriableObject.h"
#include "CarryingComponent.generated.h"

class UCarryingSettings;

UENUM()
enum class ECarryingState : uint8
{
	None = 0,
	Started = 1,
	Stopped = 2,
	Interruped = 3
};

// jesus christ this is so fucking retarted to put character in params
DECLARE_MULTICAST_DELEGATE_TwoParams(FCarryingStateChangedEvent, ABaseHumanoidCharacter* CharacterOwner, ECarryingState NewState);

// Carried item must implement ICarriableObject
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UCarryingComponent : public UActorComponent
{
	GENERATED_BODY()
friend UAnimNotify_CarryingStateChanged;
	
public:
	bool StartCarrying(AActor* Item);
	bool StopCarrying();
	bool InterruptCarrying();
	
	bool StartPutAtLocation(FVector Location);
	
	mutable FCarryingStateChangedEvent CarryingStateChangedEvent;

	bool IsCarrying() const { return bCarrying; }
	ECarryingType GetCarryingType() const { return bCarrying ? CarryingType : ECarryingType::None; }

	const UCarryingSettings* GetSettings() const { return CarryingSettings; }
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCarryingSettings* CarryingSettings;
	
private:
	bool bCarrying = false;
	bool bInteractionInProcess = false;
	bool bAttached = false;
	UPROPERTY()
	AActor* CarriedItem = nullptr;
	
	void ReleaseItem();
	void AttachItemToCharacter();
	void FinishPickingUp();
	void DetachItemFromCharacter();
	void FinishReleasingItem();
	void FinishPutInLocation();
	void ResetCarryingState(ECarryingState Reason);
	
	UPROPERTY()
	class ABaseHumanoidCharacter* HumanoidCharacter = nullptr;

	FTimerHandle Timer;
	FVector LocationToPutActor;
	ECarryingType CarryingType = ECarryingType::None;
};
