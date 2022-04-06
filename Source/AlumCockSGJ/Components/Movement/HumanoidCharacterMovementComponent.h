#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterMovementComponent.h"
#include "Data/Movement/Posture.h"
#include "Data/Movement/GCMovementMode.h"

#include "Data/Movement/MantlingMovementParameters.h"
#include "Data/Side.h"
#include "Data/Movement/SlideData.h"
#include "Data/Movement/StopClimbingMethod.h"
#include "Data/Movement/WakeUpParams.h"
#include "Data/Movement/WallrunData.h"
#include "Data/Movement/ZiplineParams.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HumanoidCharacterMovementComponent.generated.h"

DECLARE_DELEGATE_OneParam(FCrouchedOrProned, float HalfHeightAdjust)
DECLARE_DELEGATE_OneParam(FWokeUp, float HalfHeightAdjust)
DECLARE_DELEGATE(FClimbableTopReached)
DECLARE_DELEGATE_OneParam(FStoppedClimbing, const class AInteractiveActor* Interactable)
DECLARE_MULTICAST_DELEGATE_OneParam(FWallrunBegin, const ESide WallrunSide)
DECLARE_MULTICAST_DELEGATE_OneParam(FWallrunEnd, const ESide WallrunSide)
DECLARE_DELEGATE_OneParam(FZiplineObstacleHit, FHitResult Hit);
DECLARE_DELEGATE_TwoParams(FSlidingStateChangedEvent, bool bSliding, float HalfHeightAdjust)

class ALadder;

class FHumanoidCharacterSavedMove : public FBaseSavedMove
{
	
public:
	virtual void Clear() override;
	virtual uint8 GetCompressedFlags() const override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const override;
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
	virtual void PrepMoveFor(ACharacter* Character) override;
	
private:
	uint8 bSavedSprinting:1;
	uint8 bSavedMantling:1;
};

class FNetworkPredictionData_Client_HumanoidCharacter : public FNetworkPredictionData_Client_Character
{
	typedef FNetworkPredictionData_Client_Character Super;
public:
	FNetworkPredictionData_Client_HumanoidCharacter(const UCharacterMovementComponent& CMC)
		: Super(CMC)
	{
	}

	virtual FSavedMovePtr AllocateNewMove() override;
};

UCLASS()
class ALUMCOCKSGJ_API UHumanoidCharacterMovementComponent : public UBaseCharacterMovementComponent
{
	GENERATED_BODY()

friend FHumanoidCharacterSavedMove;
	
public:
	void InitPostureHalfHeights();
	virtual void BeginPlay() override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual bool CanAttemptJump() const override { return Super::CanAttemptJump() || IsSliding(); }
	bool TryStartSprint();
	void StopSprint(); 
	bool IsSprinting() const { return bSprinting; }

	void SetIsOutOfStamina(bool bNewOutOfStamina);
	void SetIsAiming(bool bNewState) { bAiming = bNewState; }

	void ToggleWalking();

#pragma region CROUCH/PRONE
	
	virtual bool IsCrouching() const override { return CurrentPosture == EPosture::Crouching; }
	bool IsProning() const { return CurrentPosture == EPosture::Proning; } 

	virtual float GetMaxSpeed() const override;
	void RequestProne();
	void RequestStandUp();
	void RequestCrouch();
	virtual void Crouch(bool bClientSimulation = false) override;
	virtual void UnCrouch(bool bClientSimulation = false) override;
	void SetAimingSpeed(float AimMaxSpeed) {CurrentAimSpeed = AimMaxSpeed; }
	void ResetAimingSpeed();
	void SetHumanoidCharacterSprintingState(uint8 NewValue);

	FCrouchedOrProned CrouchedOrProned;
	FWokeUp WokeUp;
	FSlidingStateChangedEvent SlidingStateChangedEvent;
	bool bWalking = false;

#pragma endregion CROUCH/PRONE

	void SetForwardInput(float Value) { CurrentForwardInput = Value; }
	
	bool TryStartMantle(const FMantlingMovementParameters& NewMantlingParameters);
	void EndMantle();
	bool IsMantling() const;

#pragma region CLIMB

	float GetActorToClimbableProjection(const ALadder* Ladder, const FVector& ActorLocation) const;
	bool TryStartClimbing(const ALadder* Ladder);
	void StopClimbing(EStopClimbingMethod StopClimbingMethod = EStopClimbingMethod::Fall);
	bool IsClimbing() const;
	const ALadder* GetCurrentClimbable() const { return CurrentClimbable; }
	float GetClimbingSpeedRatio() const;
	FClimbableTopReached ClimbableTopReached;
	FStoppedClimbing StoppedClimbing;
	
#pragma endregion 

#pragma region ZIPLINE

	FZiplineObstacleHit ZiplineObstacleHit;
	bool IsZiplining() const;
	bool TryStartZiplining(const FZiplineParams& NewZiplineParams);
	void StopZiplining();
	const FZiplineParams& GetZiplineParams() const { return ZiplineParams; }

#pragma endregion ZIPLINE

	bool IsSliding() const;
	bool TryStartSliding();
	void StopSliding();
	void ResetFromSliding();
	void ResetSlide();

#pragma region WALLRUN

	void RequestWallrunning();
	void StopWallrunning(bool bResetTimer);
	bool IsWallrunning() const;
	ESide GetWallrunSide() const { return WallrunData.Side; }
	const FWallrunData& GetWallrunData() const { return WallrunData; }
	void JumpOffWall();

	FWallrunBegin WallrunBeginEvent;
	FWallrunEnd WallrunEndEvent;
	
#pragma endregion 

	const EPosture& GetCurrentPosture() const { return CurrentPosture; }
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region REPLICATION

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

#pragma endregion REPLICATION
	
	
protected:
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	const class UClimbingSettings* ClimbingSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	const class UWallrunSettings* WallrunSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	const class USlideSettings* SlideSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	const class UZiplineSettings* ZiplineSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	const class UMovementSettings* MovementSettings;
	
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;	

	virtual bool IgnorePhysicsRotation() override;
	
private:
	/* State that affects capsule half height */
	UPROPERTY(Replicated)
	EPosture CurrentPosture = EPosture::Standing;
	
	float CurrentForwardInput = 0.f;
	float DefaultWalkSpeed = 0.f;
	float CurrentAimSpeed = 0.f;
	
	bool bSprinting = false;

	bool bOutOfStamina = false;
	bool bWantsToProne = false;
	bool bAiming = false;
	
	void Prone();
	void UnProne();
	bool CanProne();
	
	bool TryCrouchOrProne(float NewCapsuleHalfHeight, float NewCapsuleRadius, float& ScaledHalfHeightAdjust);
	void FillWakeUpParams(FWakeUpParams& WakeUpParams) const;
	bool TryWakeUpToState(EPosture DesiredPosture, bool bClientSimulation = false);
	bool TryWakeUp(float DesiredHalfHeight, const FWakeUpParams& WakeUpParams, float& ScaledHalfHeightAdjust, bool bClientSimulation = false);

	FMantlingMovementParameters MantlingParameters;
	FZiplineParams ZiplineParams;
	FWallrunData WallrunData;
	FSlideData SlideData;
	
	FTimerHandle MantlingTimerHandle;
	const ALadder* CurrentClimbable = nullptr;

	void PhysCustomMantling(float DeltaTime, int32 Iterations);
	void PhysCustomClimbing(float DeltaTime, int32 Iterations);
	void PhysCustomWallRun(float DeltaTime, int32 iterations);
	void PhysCustomZiplining(float DeltaTime, int32 Iterations);
	void PhysCustomSliding(float DeltaTime, int32 Iterations);
	
	TWeakObjectPtr<class ABaseHumanoidCharacter> HumanoidCharacter;

	FRotator ForceTargetRotation = FRotator::ZeroRotator;
	bool bForceRotation = false;

	TTuple<FVector, ESide> GetWallrunBeginParams();
	FVector GetWallrunSurfaceNormal(const ESide& Side, const FVector& CharacterLocationDelta = FVector::ZeroVector) const;
	ESide GetWallrunSideFromNormal(const FVector& Normal) const;

	UFUNCTION()
	void OnPlayerCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	    FVector NormalImpulse, const FHitResult& Hit);

	bool IsSurfaceWallrunnable(const FVector& SurfaceNormal) const;

	mutable class UDebugSubsystem* DebugSubsystem;
	UDebugSubsystem* GetDebugSubsystem() const;
	bool IsInCustomMovementMode(const EGCMovementMode& Mode) const;

	EMovementMode GetMovementMode();
	
	TMap<EPosture, float> PostureCapsuleHalfHeights;
};