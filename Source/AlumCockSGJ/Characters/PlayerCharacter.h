#pragma once

#include "CoreMinimal.h"
#include "BaseHumanoidCharacter.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "AI/Interfaces/NpcInteractableActor.h"
#include "Components/TimelineComponent.h"
#include "Components/Character/PlayerVisionComponent.h"
#include "Data/PlayerNpcInteractionData.h"
#include "Data/DataAssets/Characters/PlayerCharacterDataAsset.h"

#include "PlayerCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeath);

UCLASS(Blueprintable)
class ALUMCOCKSGJ_API APlayerCharacter : public ABaseHumanoidCharacter, public INpcInteractableActor, public INpcCharacter
{
    GENERATED_BODY()

public:
    APlayerCharacter(const FObjectInitializer& ObjectInitializer);
	
	UCharacterAttributesComponent* GetPlayerCharacterAttributesComponent() const { return HumanoidCharacterAttributesComponent.Get(); }
    UPlayerVisionComponent* GetCharacterVisionComponent() const { return CharacterVisionComponent; }
    UCameraComponent* GetCameraComponent() const { return CameraComponent; }
    mutable FOnDeath OnDeath;
	
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void LookUp(float Value) override;
    virtual void Turn(float Value) override;
    virtual void LookUpAtRate(float Value) override;
    virtual void TurnAtRate(float Value) override;
    virtual void MoveForward(float Value) override;
    virtual void MoveRight(float Value) override;

    virtual void SwimForward(float Value) override;
    virtual void SwimRight(float Value) override;
    virtual void SwimUp(float Value) override;

	virtual void Climb(float Value) override;
	virtual void OnJumped_Implementation() override;

	virtual void OnOutOfHealth() override;

	virtual void Interact() override;

	virtual bool TryStartNpcInteraction_Implementation(ABaseCharacter* Npc, const FGameplayTag& InteractionTag) override;
	virtual FNpcInteractionStopResult
	StopNpcInteraction_Implementation(ABaseCharacter* Npc, bool bInterupted) override;
	virtual bool IsNpcInteractionAvailable_Implementation(const FGameplayTag& InteractionTag) const override;

	virtual const FDataTableRowHandle& GetNpcDTRH() const override { return PlayerDTRH; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UPlayerVisionComponent* CharacterVisionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(RowType="NpcDTR"))
	FDataTableRowHandle PlayerDTRH;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagQuery UninteractableStates;
	
	virtual void OnSprintStart_Implementation() override;
	virtual void OnSprintEnd_Implementation() override;

    virtual void OnStartCrouchOrProne(float HalfHeightAdjust) override;
    virtual void OnEndCrouchOrProne(float HalfHeightAdjust) override;
    virtual void OnSlidingStateChangedEvent(bool bSliding, float HalfHeightAdjust) override;
	
    virtual void OnAimingStart_Implementation(float FOV, float NewTurnModifier, float NewLookUpModifier) override;
    virtual void OnAimingEnd_Implementation() override;
	
private:
    FTimeline SprintSpringArmTimeline;
    FTimeline AimFovAdjustmentTimeline;

    float DefaultSpringArmOffset = 0.f;

    void InitTimeline(FTimeline& Timeline, UCurveFloat* Curve, void (APlayerCharacter::*Callback)(float) const);
    void AdjustSpringArm(const FVector& Adjustment);
    void AdjustSpringArmRelative(const FVector& Adjustment);
    void OnWallrunBegin(ESide Side);
    void OnWallrunEnd(ESide Side);
    void OnWallrunChanged(ESide Side, int AdjustmentModification);
    float DefaultFOV = 0.f;

    float TurnModifier = 1.f;
    float LookUpModifier = 1.f;
    float AimingFOV = 0.f;

    void SetAimFovPosition(float Alpha) const;
    void SetSpringArmPosition(float Alpha) const;

    TArray<FTimeline*> ActiveTimelines;
	TWeakObjectPtr<const class UPlayerCharacterDataAsset> PlayerCharacterSettings;
	FPlayerNpcInteractionData PlayerNpcInteractionData;
};