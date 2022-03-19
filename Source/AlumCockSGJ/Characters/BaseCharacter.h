#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Actors/CommonDelegates.h"
#include "AI/Characters/Turret.h"
#include "Components/Character/BaseCharacterAttributesComponent.h"
#include "AI/Data/AITypesGC.h"
#include "Data/CharacterInteractionParameters.h"
#include "Data/DataAssets/Characters/BaseCharacterDataAsset.h"
#include "Data/Entities/SpawnItemDescription.h"
#include "GameFramework/Character.h"
#include "Interfaces/GameplayTagActor.h"
#include "Interfaces/Killable.h"
#include "SaveSystem/SaveSubsystemInterface.h"
#include "BaseCharacter.generated.h"

class UFMODEvent;
class UCharacterEquipmentComponent;
class UBaseCharacterAttributesComponent;
class UCharacterCombatComponent;
class UCharacterInteractionOptions;
class ABaseCharacter;

DECLARE_MULTICAST_DELEGATE_OneParam(FCharacterGameplayStateChanged, const FGameplayTagContainer& NewState);
DECLARE_MULTICAST_DELEGATE_OneParam(FCharacterDeathEvent, const ABaseCharacter* DeadCharacter);

UCLASS()
class ALUMCOCKSGJ_API ABaseCharacter : public ACharacter, public IGenericTeamAgentInterface, public IKillable,
	public IGameplayTagActor, public IGameplayTagAssetInterface, public ISaveSubsystemInterface 
{
	GENERATED_BODY()

friend UCharacterEquipmentComponent;
friend UCharacterCombatComponent;
friend class UNpcBaseLogicComponent;
friend class UNpcHumanoidLogicComponent;

public:
	ABaseCharacter(const FObjectInitializer& ObjectInitializer);
	// mutable FReportKillEvent ReportKillEvent;
	mutable FInputLockedEvent InputLockedEvent;
	mutable FCharacterGameplayStateChanged CharacterGameplayStateChangedEvent;
	mutable FCharacterDeathEvent CharacterDeathEvent;
	
    virtual  void OnLevelDeserialized_Implementation() override;

#pragma region INPUT
	virtual void Turn(float Value) {}
	virtual void LookUp(float Value) {}
	virtual void TurnAtRate(float Value) {}
	virtual void LookUpAtRate(float Value) {}
	virtual void MoveForward(float Value);
	virtual void MoveRight(float Value) { CurrentInputRight = Value; }

	void StartFiring();
	void StopFiring();
	void StartAiming();
	void StopAiming();
	void StartReloading();
	void ToggleFireMode();

	void ChangeWeapon(int WeaponIndexDelta);
	void ThrowItem();

	void StartPrimaryMeleeAttack();
	void StopPrimaryMeleeAttack();
	void StartSecondaryMeleeAttack();
	void StopSecondaryMeleeAttack();
	
#pragma endregion INPUT

	UBaseCharacterAttributesComponent* GetBaseCharacterAttributesComponent() const { return CharacterAttributesComponent; }
	UCharacterEquipmentComponent* GetEquipmentComponent () const { return CharacterEquipmentComponent; }
	UCharacterCombatComponent* GetCombatComponent() const { return CharacterCombatComponent; }
	
	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId((uint8)Team); }
	virtual bool IsAlive() const override { return CharacterAttributesComponent->IsAlive(); }
	ETeam GetTeam() const { return Team; }
	// virtual void ReportKill(AActor* KilledActor);
	float GetCurrentInputForward() const { return CurrentInputForward; }
	float GetCurrentInputRight() const { return CurrentInputRight; }
	virtual bool IsConsumingStamina() const;
	void UpdateStrafingControls();
	virtual bool SetDesiredRotation(const FRotator& Rotator);
	void UnsetDesiredRotation();
	virtual FCharacterInteractionParameters GetInteractionParameters(const FGameplayTag& InteractionTag) const;
	
	UPROPERTY(SaveGame)
    bool bIsLoadoutCreated=false;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual void ChangeGameplayTags(const FGameplayTagContainer& TagsContainer, bool bAdd = true) override;

	// Returns expected duration seconds
	UFUNCTION(BlueprintCallable)
	float PlayFmodEvent(UFMODEvent* FmodEvent);

	UFUNCTION(BlueprintCallable)
	void InteruptVoiceLine();

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UBaseCharacterAttributesComponent* CharacterAttributesComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UCharacterEquipmentComponent* CharacterEquipmentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UCharacterCombatComponent* CharacterCombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UFMODAudioComponent* FmodAudioComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBaseCharacterDataAsset* CharacterSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCharacterInteractionOptions* InteractionOptions;

	class UBaseCharacterMovementComponent* BaseMovementComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
	ETeam Team = ETeam::GoodGuys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FSpawnItemDescription> SpawnItemsAfterDeath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer GameplayStateTags;
	
	void SpawnItems(const TArray<FSpawnItemDescription>& SpawnItemsList);
	virtual void SpawnItemsOnDeath();
	virtual void OnOutOfHealth();

	UFUNCTION()
	virtual void ReactToDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
		AController* InstigatedBy, AActor* DamageCauser);
	virtual void SetStrafingControlsState(bool bStrafing);

	float PlayAnimMontageWithDuration(UAnimMontage* Montage, float DesiredDuration) const;
	
	float CurrentInputForward = 0.f;
	float CurrentInputRight = 0.f;
	float FallApexWorldZ = 0.f;
	void EnableRagdoll() const;

	virtual bool CanStartAction(ECharacterAction Action);
	virtual void HandleCancellingActions(ECharacterAction ActionToInterrupt);
	virtual void OnActionStarted(ECharacterAction Action);
	virtual void OnActionEnded(ECharacterAction Action);
	TSet<ECharacterAction> ActiveActions;

	virtual bool IsPreferStrafing() const;
	virtual void PossessedBy(AController* NewController) override;

	virtual bool CanReload() const;
	virtual bool CanShoot() const;

	virtual UCharacterInteractionOptions* GetInteractionOptions() const { return InteractionOptions; }
	bool bSprintRequested = false;

private:
    void CreateLoadout();
};
