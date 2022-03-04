#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Actors/CommonDelegates.h"
#include "Components/Character/CharacterAttributesComponent.h"
#include "Data/CharacterTypes.h"
#include "Data/Movement/MantlingSettings.h"
#include "Data/Movement/ZiplineParams.h"

#include "Actors/Interactive/Environment/Zipline.h"
#include "Animations/HumanoidCharacterAnimInstance.h"
#include "Components/Character/CarryingComponent.h"
#include "Data/MontagePlayResult.h"
#include "Components/Movement/HumanoidCharacterMovementComponent.h"
#include "Data/DataAssets/Characters/HumanoidCharacterDataAsset.h"
#include "GameFramework/Character.h"
#include "Interfaces/Soakable.h"
#include "BaseHumanoidCharacter.generated.h"

class USoakingComponent;
class AEquippableItem;
class ARangeWeaponItem;
class UHumanoidCharacterMovementComponent;
class UInverseKinematicsComponent;
class UCharacterEquipmentComponent;
class UCharacterAttributesComponent;
class AInteractiveActor;

UCLASS(Abstract)
class ALUMCOCKSGJ_API ABaseHumanoidCharacter : public ABaseCharacter, public ISoakable
{
	GENERATED_BODY()
// :))
friend UHumanoidCharacterMovementComponent;
friend UHumanoidCharacterAnimInstance;
friend UCarryingComponent;
	
public:
	ABaseHumanoidCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UInverseKinematicsComponent* GetInverseKinematicsComponent() const { return InverseKinematicsComponent; }
	const UHumanoidCharacterMovementComponent* GetGCMovementComponent () const { return HumanoidMovementComponent.Get(); }
	USoakingComponent* GetSoakingComponent() const { return SoakingComponent; }
	class UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	UCarryingComponent* GetCarryingComponent() const { return CarryingComponent; }
	
	mutable FAmmoChangedEvent AmmoChangedEvent;

#pragma region INPUT
	
	virtual void Interact();

	UFUNCTION(BlueprintCallable)
	virtual void Mantle(bool bForce = false);

	virtual void StartRequestingSprint();
	virtual void StopRequestingSprint();
	virtual void StopRequestingWallrun();
	virtual void StartRequestingWallrun();
	virtual void ToggleCrouchState();
	virtual void SwimForward(float Value){ CurrentInputForward = Value; }
	virtual void SwimRight(float Value) { CurrentInputRight = Value; }
	virtual void SwimUp(float Value) {  }
	virtual void MoveForward(float Value) override;
	virtual void Jump() override;
	virtual void ClimbUp(float Value) { CurrentInputForward = Value; }
	virtual void ClimbDown(float Value) { CurrentInputForward = Value; }

	void StopZiplining();
	
#pragma endregion 
	
	virtual void OnJumped_Implementation() override;

	void ToggleProneState();
	
	void RegisterInteractiveActor(const AInteractiveActor* InteractiveActor);
	void UnregisterInteractiveActor(const AInteractiveActor* InteractiveActor);
	
	void TryStartSliding();
	void StopSliding(bool bForce = false);
	
	virtual void StartSoaking() override;
	virtual void StopSoaking() override;

	virtual bool IsConsumingStamina() const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	UInverseKinematicsComponent* InverseKinematicsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	ULedgeDetectionComponent* LedgeDetectionComponent; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UInventoryComponent* InventoryComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USoakingComponent* SoakingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UCarryingComponent* CarryingComponent;
	
	virtual bool CanSprint() const;
	void TryStartSprinting();
	void StopSprinting();

	UFUNCTION(BlueprintNativeEvent, Category = "Character|Movement")
	void OnSprintStart();
	virtual void OnSprintStart_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Character|Movement")
	void OnSprintEnd();
	virtual void OnSprintEnd_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Character|Aiming")
	void OnAimingStart(float FOV, float TurnModifier, float LookUpModifier);
	virtual void OnAimingStart_Implementation(float FOV, float TurnModifier, float LookUpModifier) {}

	UFUNCTION(BlueprintNativeEvent, Category = "Character|Aiming")
	void OnAimingEnd();
	virtual void OnAimingEnd_Implementation() {}
	
	virtual void Falling() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void NotifyJumpApex() override;
	virtual void OnStartCrouchOrProne(float HalfHeightAdjust);
	virtual void OnEndCrouchOrProne(float HalfHeightAdjust);
	virtual void OnSlidingStateChangedEvent(bool bSliding, float HalfHeightAdjust);
	bool IsPendingMovement() const { return CurrentInputForward != 0 || CurrentInputRight != 0; }

	virtual void OnClimbableTopReached();
	virtual void OnStoppedClimbing(const AInteractiveActor* Interactable);
	virtual void OnZiplineObstacleHit(FHitResult Hit);
	bool CanMantle() const;

	virtual bool CanAttemptWallrun() const;

	TWeakObjectPtr<UHumanoidCharacterMovementComponent> HumanoidMovementComponent = nullptr;

	virtual bool CanJumpInternal_Implementation() const override;

	virtual void PlayMantleMontage(const FMantlingSettings& MantleSettings, float StartTime);
	virtual FMontagePlayResult PlayHardLandMontage();
	FMontagePlayResult PlayHardLandMontage(UAnimInstance* AnimInstance, UAnimMontage* AnimMontage) const;

	FOnMontageEnded MontageEndedUnlockControlsEvent;
	void OnMontageEndedUnlockControls(UAnimMontage* AnimMontage, bool bInterrupted);
	void OnMantleEnded();
	void SetInputDisabled(bool bDisabledMovement, bool bDisabledCamera);
	
	TWeakObjectPtr<const AInteractiveActor> CurrentInteractable = nullptr;
	
	void StopWallrunning();
	virtual void OnOutOfStamina(bool bOutOfStamina);
	
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	
	TWeakObjectPtr<UCharacterAttributesComponent> HumanoidCharacterAttributesComponent;

	virtual void HandleCancellingActions(ECharacterAction ActionToInterrupt) override;
	virtual bool IsPreferStrafing() const override;
	virtual void OnDesiredRotationReached();

	virtual bool CanReload() const override;
	virtual bool CanShoot() const override;

	FVector TurnHeadTowards = FVector::ZeroVector;
	
private:
	void TryChangeSprintState();
	
	const FMantlingSettings& GetMantlingSettings(float Height) const;
	FZiplineParams GetZipliningParameters(const AZipline* Zipline) const;
	void UpdateSuffocatingState();

	bool TryStartInteracting();
	void TryStopInteracting();
	void ResetInteraction();
	const AInteractiveActor* GetPreferableInteractable();
	bool bInteracting = false;

	TArray<const AInteractiveActor*, TInlineAllocator<8>> InteractiveActors;

	bool CanSlide() const { return HumanoidMovementComponent->IsSprinting() && HumanoidCharacterSettings->bCanSlide; }
	
	void ChangeMeshOffset(float HalfHeightAdjust);

	void OnAimStateChanged(bool bAiming, ARangeWeaponItem* Weapon);

	TWeakObjectPtr<const UHumanoidCharacterDataAsset> HumanoidCharacterSettings;
};
