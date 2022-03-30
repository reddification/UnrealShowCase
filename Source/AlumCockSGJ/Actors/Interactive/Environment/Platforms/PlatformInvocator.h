#pragma once

#include "CoreMinimal.h"
#include "PlatformInvocator.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInvocatorActivated, bool bInvoked);

UCLASS()
class ALUMCOCKSGJ_API APlatformInvocator : public AActor
{
	GENERATED_BODY()
	
public:
	APlatformInvocator();
	mutable FOnInvocatorActivated OnInvocatorActivated;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* TriggerBox;

	UPROPERTY(ReplicatedUsing=OnRep_IsActivated)
	bool bIsActivated = false;

private:
	UPROPERTY()
	TArray<APawn*> OverlappedPawns;

	UFUNCTION()
	void OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void OnIsActivated(bool bIsActivated_In);
	
	UFUNCTION()
	void OnRep_IsActivated(bool bIsActivatedPrevious);

	// not gonna work because Platform belongs to server and not to the autonomous proxy/ local authority
	// UFUNCTION(Server, Reliable)
	// void Server_SetIsActivated(bool bIsActivated_In);
	
	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void Multicast_SetIsActivated(bool bIsActivated_In);
	
	void Multicast_SetIsActivated_Implementation(bool bIsActivated_In);
	bool Multicast_SetIsActivated_Validate(bool bIsActivated_In);
};
