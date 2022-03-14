#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Actors/Interactive/Environment/Ladder.h"
#include "Components/Movement/HumanoidCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Game/MLGameModeBase.h"
#include "Game/MLCoreTypes.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/Controllers/BasePlayerController.h"
#include "Components/Character/CharacterEquipmentComponent.h"
#include "Components/Character/PlayerVisionComponent.h"
#include "Components/Character/SoakingComponent.h"
#include "Components/Combat/CharacterCombatComponent.h"
#include "Data/DataAssets/Characters/PlayerCharacterDataAsset.h"
#include "Subsystems/NpcSubsystem.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->bUsePawnControlRotation = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(SpringArmComponent);
    CameraComponent->bUsePawnControlRotation = false;
    
    CharacterVisionComponent = CreateDefaultSubobject<UPlayerVisionComponent>(TEXT("Vision"));
    // CharacterVisionComponent->SetupAttachment(GetMesh(), "head");
    CharacterVisionComponent->SetupAttachment(GetMesh());
    
    GetCharacterMovement()->bOrientRotationToMovement = 1;
    GetCharacterMovement()->JumpZVelocity = 600.f;

    DefaultSpringArmOffset = SpringArmComponent->TargetArmLength;
}

// void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
// {
//     Super::SetupPlayerInputComponent(PlayerInputComponent);

    // FInputActionBinding Binding("Jump", IE_Pressed);
    // Binding.ActionDelegate.BindDelegate(this, &APlayerCharacter::Jump);
    // InputComponent->AddActionBinding(Binding);
    // EnableInput(StaticCast<APlayerController*>(Controller));

    // I wish it fucking worked
    // PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::Turn);
    // PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);
    // PlayerInputComponent->BindAxis("TurnAtRate", this, &APlayerCharacter::TurnAtRate);
    // PlayerInputComponent->BindAxis("LookUpAtRate", this, &APlayerCharacter::LookUpAtRate);
    // PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
    // PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
    //
    // PlayerInputComponent->BindAction("Mantle", IE_Pressed, this, &APlayerCharacter::Mantle);
    // PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::Jump);
    //
    // PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::StartRequestingSprint);
    // PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopRequestingSprint);
    //
    // PlayerInputComponent->BindAxis("SwimForward", this, &APlayerCharacter::SwimForward);
    // PlayerInputComponent->BindAxis("SwimRight", this, &APlayerCharacter::SwimRight);
    // PlayerInputComponent->BindAxis("SwimUp", this, &APlayerCharacter ::SwimUp);
    //
    // PlayerInputComponent->BindAxis("Climb", this, &APlayerCharacter::Climb);
    //
    // PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::Interact);
    //
    // PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &APlayerCharacter::TryStartSliding);
    // PlayerInputComponent->BindAction("Slide", IE_Released, this, &APlayerCharacter::StopSliding);
    // PlayerInputComponent->BindAction("Prone", IE_Pressed, this, &APlayerCharacter::ToggleProneState);
    // PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::ToggleCrouchState);
    //
    // PlayerInputComponent->BindAction("Wallrun", IE_Pressed, this, &APlayerCharacter::StartRequestingWallrun);
    // PlayerInputComponent->BindAction("Wallrun", IE_Released, this, &APlayerCharacter::StopRequestingWallrun);
    //
    // PlayerInputComponent->BindAction("Fire", IE_Pressed, CharacterCombatComponent, &UCharacterCombatComponent::StartFiring);
    // PlayerInputComponent->BindAction("Fire", IE_Released, CharacterCombatComponent, &UCharacterCombatComponent::StopFiring);
    //
    // PlayerInputComponent->BindAction("Aim", IE_Pressed, CharacterCombatComponent, &UCharacterCombatComponent::StartAiming);
    // PlayerInputComponent->BindAction("Aim", IE_Released, CharacterCombatComponent, &UCharacterCombatComponent::StopAiming);
    //
    // PlayerInputComponent->BindAction("Reload", IE_Pressed, CharacterEquipmentComponent, &UCharacterEquipmentComponent::TryReload);
    //
    // PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, CharacterEquipmentComponent, &UCharacterEquipmentComponent::PickNextWeapon);
    // PlayerInputComponent->BindAction("PreviousWeapon", IE_Pressed, CharacterEquipmentComponent, &UCharacterEquipmentComponent::PickPreviousWeapon);
    //
    // PlayerInputComponent->BindAction("ThrowItem", IE_Pressed, CharacterCombatComponent, &UCharacterCombatComponent::TryThrow);
    //
    // PlayerInputComponent->BindAction("PrimaryMeleeAttack", IE_Pressed, CharacterCombatComponent, &UCharacterCombatComponent::StartPrimaryMeleeAttack);
    // PlayerInputComponent->BindAction("PrimaryMeleeAttack", IE_Released, CharacterCombatComponent, &UCharacterCombatComponent::StopPrimaryMeleeAttack);
    //
    // PlayerInputComponent->BindAction("SecondaryMeleeAttack", IE_Pressed, CharacterCombatComponent, &UCharacterCombatComponent::StartHeavyMeleeAttack);
    // PlayerInputComponent->BindAction("SecondaryMeleeAttack", IE_Released, CharacterCombatComponent, &UCharacterCombatComponent::StopHeavyMeleeAttack);
    //
    // PlayerInputComponent->BindAction("ToggleFireMode", IE_Pressed, CharacterEquipmentComponent, &UCharacterEquipmentComponent::StartTogglingFireMode);
    //
    // PlayerInputComponent->BindAction("UnequipWeapon", IE_Pressed, CharacterEquipmentComponent, &UCharacterEquipmentComponent::UnequipWeapon);
    // PlayerInputComponent->BindAction("EquipPrimaryWeapon", IE_Pressed, CharacterEquipmentComponent, &UCharacterEquipmentComponent::EquipPrimaryWeapon);
    // PlayerInputComponent->BindAction("EquipSecondaryWeapon", IE_Pressed, CharacterEquipmentComponent, &UCharacterEquipmentComponent::EquipSecondaryWeapon);
    // PlayerInputComponent->BindAction("EquipMeleeWeapon", IE_Pressed, CharacterEquipmentComponent, &UCharacterEquipmentComponent::EquipMeleeWeapon);
    // PlayerInputComponent->BindAction("EquipPrimaryThrowable", IE_Pressed, CharacterEquipmentComponent, &UCharacterEquipmentComponent::EquipPrimaryThrowable);
    // PlayerInputComponent->BindAction("EquipSecondaryThrowable", IE_Pressed, CharacterEquipmentComponent, &UCharacterEquipmentComponent::EquipSecondaryThrowable);
// }

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    checkf(IsValid(CharacterSettings) && CharacterSettings->IsA<UPlayerCharacterDataAsset>(), TEXT("Character settings must be a valid UPlayerCharacterDataAsset"));
    PlayerCharacterSettings = StaticCast<const UPlayerCharacterDataAsset*>(CharacterSettings);
    DefaultFOV = CameraComponent->FieldOfView;
    InitTimeline(SprintSpringArmTimeline, PlayerCharacterSettings->SprintSpringArmTimelineCurve, &APlayerCharacter::SetSpringArmPosition);
    InitTimeline(AimFovAdjustmentTimeline, PlayerCharacterSettings->AimFovAdjustmentCurve, &APlayerCharacter::SetAimFovPosition);
    HumanoidMovementComponent->WallrunBeginEvent.AddUObject(this, &APlayerCharacter::OnWallrunBegin);
    HumanoidMovementComponent->WallrunEndEvent.AddUObject(this, &APlayerCharacter::OnWallrunEnd);
    auto NpcSubsystem = GetWorld()->GetSubsystem<UNpcSubsystem>();
    NpcSubsystem->RegisterNpc(PlayerDTRH, this);
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    for (FTimeline* ActiveTimeline : ActiveTimelines)
    {
        if (ActiveTimeline->IsPlaying() || ActiveTimeline->IsReversing())
        {
            ActiveTimeline->TickTimeline(DeltaSeconds);
        }
    }
}

void APlayerCharacter::InitTimeline(FTimeline& Timeline, UCurveFloat* Curve, void (APlayerCharacter::*Callback)(float) const)
{
    if (IsValid(Curve))
    {
        FOnTimelineFloatStatic TimelineCallback;
        TimelineCallback.BindUObject(this, Callback);
        Timeline.AddInterpFloat(Curve, TimelineCallback);
        ActiveTimelines.Add(&Timeline);
    }
}

void APlayerCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value * LookUpModifier);
}

void APlayerCharacter::Turn(float Value)
{
    AddControllerYawInput(Value * TurnModifier);
}

void APlayerCharacter::LookUpAtRate(float Value)
{
    AddControllerPitchInput(Value * PlayerCharacterSettings->BaseLookUpRate * LookUpModifier * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::TurnAtRate(float Value)
{
    AddControllerPitchInput(Value * PlayerCharacterSettings->BaseTurnRate * TurnModifier * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::MoveForward(float Value)
{
    Super::MoveForward(Value);
    
    if (!GetMovementComponent()->IsFalling() && !GetMovementComponent()->IsMovingOnGround())
    {
        return;
    }

    if (!FMath::IsNearlyZero(Value, SMALL_NUMBER))
    {
        const FRotator Rotator(0, GetControlRotation().Yaw, 0);
        FVector ForwardVector = Rotator.RotateVector(FVector::ForwardVector);
        AddMovementInput(ForwardVector, Value);
    }
}

void APlayerCharacter::MoveRight(float Value)
{
    Super::MoveRight(Value);
    if (!GetMovementComponent()->IsFalling() && !GetMovementComponent()->IsMovingOnGround())
    {
        return;
    }

    if (!FMath::IsNearlyZero(Value, SMALL_NUMBER))
    {
        const FRotator Rotator(0, GetControlRotation().Yaw, 0);
        FVector RightVector = Rotator.RotateVector(FVector::RightVector);
        AddMovementInput(RightVector, Value);
    }
}

void APlayerCharacter::SwimForward(float Value)
{
    if (!GetMovementComponent()->IsSwimming())
    {
        return;
    }

    Super::SwimForward(Value);
    if (!FMath::IsNearlyZero(Value, SMALL_NUMBER))
    {
        const FRotator ControlRotation = GetControlRotation();
        const FRotator Rotator(ControlRotation.Pitch, ControlRotation.Yaw, 0);
        FVector ForwardVector = Rotator.RotateVector(FVector::ForwardVector);
        AddMovementInput(ForwardVector, Value);
    }
}

void APlayerCharacter::SwimRight(float Value)
{
    if (!GetMovementComponent()->IsSwimming())
    {
        return;
    }

    Super::SwimRight(Value);
    if (!FMath::IsNearlyZero(Value, SMALL_NUMBER))
    {
        const FRotator YawRotator(0, GetControlRotation().Yaw, 0);
        FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
        AddMovementInput(RightVector, Value);
    }
}

void APlayerCharacter::SwimUp(float Value)
{
    if (GetMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, SMALL_NUMBER))
    {
        FVector Direction;
        if (Value > 0)
        {
            Direction = GetActorForwardVector();
            Direction.Z = 0.99f;
        }
        else
        {
            Direction = FVector::UpVector;
        }

        AddMovementInput(Direction, Value);
    }
}

void APlayerCharacter::Climb(float Value)
{
    if (GetHumanoidMovementComponent()->IsClimbing() && !FMath::IsNearlyZero(Value))
    {
        const auto Climbable = GetHumanoidMovementComponent()->GetCurrentClimbable();
        FVector ClimbingVector = Climbable->GetActorUpVector();
        AddMovementInput(ClimbingVector, Value);
    }
}

void APlayerCharacter::AdjustSpringArm(const FVector& Adjustment)
{
    SpringArmComponent->TargetOffset += Adjustment;
}

void APlayerCharacter::AdjustSpringArmRelative(const FVector& Adjustment)
{
    SpringArmComponent->AddRelativeLocation(Adjustment);
}

void APlayerCharacter::OnJumped_Implementation()
{
    Super::OnJumped_Implementation();
    if (bIsCrouched)
    {
        UnCrouch();
    }
}

void APlayerCharacter::OnSprintStart_Implementation()
{
    if (IsValid(PlayerCharacterSettings->SprintSpringArmTimelineCurve))
    {
        SprintSpringArmTimeline.Play();
    }
}

void APlayerCharacter::OnSprintEnd_Implementation()
{
    if (IsValid(PlayerCharacterSettings->SprintSpringArmTimelineCurve))
    {
        SprintSpringArmTimeline.Reverse();
    }
}

void APlayerCharacter::OnStartCrouchOrProne(float HalfHeightAdjust)
{
    Super::OnStartCrouchOrProne(HalfHeightAdjust);
    AdjustSpringArm(FVector(0, 0, HalfHeightAdjust));
}

void APlayerCharacter::OnEndCrouchOrProne(float HalfHeightAdjust)
{
    Super::OnEndCrouchOrProne(HalfHeightAdjust);
    AdjustSpringArm(FVector(0, 0, -HalfHeightAdjust));
}

void APlayerCharacter::OnWallrunBegin(ESide Side)
{
    OnWallrunChanged(Side, 1);
}

void APlayerCharacter::OnWallrunEnd(ESide Side)
{
    OnWallrunChanged(Side, -1);
}

void APlayerCharacter::OnWallrunChanged(ESide Side, int AdjustmentModification)
{
    const float CameraPositionAdjustment = 50.f;
    switch (Side)
    {
    case ESide::Left:
        break;
    case ESide::Right:
        AdjustmentModification *= -1;
        break;
    default:
        break;
    }

    // TODO timeline curve
    AdjustSpringArmRelative(FVector(0, AdjustmentModification * CameraPositionAdjustment, 0));
}

void APlayerCharacter::OnSlidingStateChangedEvent(bool bSliding, float HalfHeightAdjust)
{
    Super::OnSlidingStateChangedEvent(bSliding, HalfHeightAdjust);
    AdjustSpringArm(FVector(0, 0, bSliding ? HalfHeightAdjust : -HalfHeightAdjust));
}

void APlayerCharacter::OnAimingStart_Implementation(float FOV, float NewTurnModifier, float NewLookUpModifier)
{
    Super::OnAimingStart_Implementation(FOV, NewTurnModifier, NewLookUpModifier);
    TurnModifier = NewTurnModifier;
    LookUpModifier = NewLookUpModifier;
    if (IsValid(PlayerCharacterSettings->AimFovAdjustmentCurve))
    {
        AimFovAdjustmentTimeline.Play();
        AimingFOV = FOV;
    }
    else
    {
        CameraComponent->SetFieldOfView(FOV);
    }
}

void APlayerCharacter::OnAimingEnd_Implementation()
{
    Super::OnAimingEnd_Implementation();
    TurnModifier = 1.f;
    LookUpModifier = 1.f;
    if (IsValid(PlayerCharacterSettings->AimFovAdjustmentCurve))
    {
        AimFovAdjustmentTimeline.Reverse();
    }
    else
    {
        CameraComponent->SetFieldOfView(DefaultFOV);
    }
}

void APlayerCharacter::SetSpringArmPosition(float Alpha) const
{
    SpringArmComponent->TargetArmLength = FMath::Lerp(DefaultSpringArmOffset, PlayerCharacterSettings->SprintSpringArmOffset, Alpha);
}

void APlayerCharacter::SetAimFovPosition(float Alpha) const
{
    CameraComponent->SetFieldOfView(FMath::Lerp(DefaultFOV, AimingFOV, Alpha));
}

void APlayerCharacter::OnOutOfHealth()
{
    Super::OnOutOfHealth();
    SetInputDisabled(true, true);

    if (GetWorld())
    {
        const auto GameMode = Cast<AMLGameModeBase>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            GameMode->SetGameState(EMLGameState::GameOver);
        }
    }

    OnDeath.Broadcast();
}

void APlayerCharacter::Interact()
{
    bool bVisualInteraction = CharacterVisionComponent->Interact();
    if (bVisualInteraction)
        return;
    
    Super::Interact();
}

bool APlayerCharacter::TryStartNpcInteraction_Implementation(ABaseCharacter* Npc, const FGameplayTag& InteractionTag)
{
    if (!IsNpcInteractionAvailable_Implementation(InteractionTag))
        return false;

    auto InteractableNpc = Cast<IInteractable>(Npc);
    if (!InteractableNpc)
        return false;
    
    return InteractableNpc->InteractWithPlayer(this);
}

bool APlayerCharacter::StopNpcInteraction_Implementation(ABaseCharacter* Npc, bool bInterupted)
{
    PlayerNpcInteractionData.Reset();
    return true;
}

bool APlayerCharacter::IsNpcInteractionAvailable_Implementation(const FGameplayTag& InteractionTag) const
{
    FGameplayTagContainer CurrentState;
    GetOwnedGameplayTags(CurrentState);
    return !PlayerNpcInteractionData.IsInteracting() && (UninteractableStates.IsEmpty() || !UninteractableStates.Matches(CurrentState));
}
