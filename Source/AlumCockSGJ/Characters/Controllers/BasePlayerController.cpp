#include "BasePlayerController.h"

#include "Actors/Equipment/Weapons/ThrowableItem.h"
#include "Components/Character/CharacterEquipmentComponent.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "Game/MLGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/PlayerCharacter.h"
#include "Components/ObservationSourceComponent.h"
#include "Components/Character/InventoryComponent.h"
#include "Components/Combat/CharacterCombatComponent.h"
#include "SaveSystem/SaveGameSubsystem.h"
#include "Subsystems/DialogueSubsystem.h"
#include "Subsystems/NpcSubsystem.h"
#include "Subsystems/QuestSubsystem.h"
#include "UI/DialogueWidget.h"
#include "UI/MLGameHUD.h"

void ABasePlayerController::SetPawn(APawn* InPawn)
{
    Super::SetPawn(InPawn);
    if (PlayerCharacter == InPawn)
        return;

    PlayerCharacter = Cast<APlayerCharacter>(InPawn);
    if (IsValid(PlayerCharacter))
    {
        const UCharacterEquipmentComponent* EquipmentComponent = PlayerCharacter->GetEquipmentComponent();
        PlayerCharacter->GetBaseCharacterAttributesComponent()->AttributeChangedEvent.AddUObject(
            this, &ABasePlayerController::OnAttributeChanged);

        EquipmentComponent->WeaponAmmoChangedEvent.BindUObject(this, &ABasePlayerController::OnAmmoChanged);
        EquipmentComponent->WeaponAmmoInfoChangedEvent.AddUObject(this, &ABasePlayerController::OnWeaponAmmoInfoChanged);
        EquipmentComponent->ThrowableEquippedEvent.BindUObject(this, &ABasePlayerController::OnThrowableEquipped);
        EquipmentComponent->ThrowablesCountChanged.BindUObject(this, &ABasePlayerController::OnThowablesCountChanged);
        EquipmentComponent->WeaponEquippedEvent.AddUObject(this, &ABasePlayerController::OnWeaponEquipped);
        EquipmentComponent->WeaponUnequippedEvent.AddUObject(this, &ABasePlayerController::OnWeaponUnequipped);
        EquipmentComponent->MeleeWeaponEquippedEvent.BindUObject(this, &ABasePlayerController::OnMeleeWeaponEquipped);

        UQuestSubsystem* QuestSubsystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>();
        QuestSubsystem->RegisterPlayerController(this);

        auto NpcSubsystem = GetWorld()->GetSubsystem<UNpcSubsystem>();
        if (NpcSubsystem)
            NpcSubsystem->RegisterPlayerController(this);
        
        PlayerCharacter->GetCombatComponent()->AimStateChangedEvent.AddUObject(this, &ABasePlayerController::OnAimingStateChanged);
        PlayerCharacter->GetInventoryComponent()->ItemPickedUpEvent.AddUObject(this, &ABasePlayerController::OnItemPickedUp);
        
        PlayerCharacter->InputLockedEvent.AddUObject(this, &ABasePlayerController::SetInputLocked);
    }
}

void ABasePlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if(IsValid(PlayerCharacter) && !PlayerCharacter->bIsCrouched)
        PlayerHUDWidget->OnPlayerSpeedChanged(GetPawn()->GetVelocity().Size());
}

void ABasePlayerController::BeginPlay()
{
    Super::BeginPlay();
    auto HUD = Cast<AMLGameHUD>(GetHUD());
    if (HUD)
        PlayerHUDWidget = HUD->GetPlayerHUD();

    if (GetWorld())
    {
        const auto GameMode = Cast<AMLGameModeBase>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            GameMode->OnGameStateChanged.AddUObject(this, &ABasePlayerController::OnGameStateChanged);
        }
    }

    UQuestSubsystem* QuestSubsystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>();
    QuestSubsystem->QuestStartedEvent.AddUObject(this, &ABasePlayerController::OnQuestStarted);
    QuestSubsystem->QuestTaskCompletedEvent.AddUObject(this, &ABasePlayerController::OnQuestEventOccured);
    QuestSubsystem->QuestCompletedEvent.AddUObject(this, &ABasePlayerController::OnQuestCompleted);
    QuestSubsystem->CutsceneStartedEvent.AddUObject(this, &ABasePlayerController::OnCutsceneStarted);
    QuestSubsystem->CutsceneEndedEvent.AddUObject(this, &ABasePlayerController::OnCutsceneEnded);
    GetWorldTimerManager().SetTimer(ObservationSourcesUpdateTimer, this, &ABasePlayerController::UpdateObservationSources,
        ObservationSourcesUpdateIntervalMilliseconds / 1000.f, true);

    auto DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
    DialogueSubsystem->RegisterPlayerController(this);
}

void ABasePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (!InputComponent)
        return;

    // ORDER MATTERS!!!
    InputComponent->BindAxis("Turn", this, &ABasePlayerController::Turn);
    InputComponent->BindAxis("LookUp", this, &ABasePlayerController::LookUp);
    InputComponent->BindAxis("TurnAtRate", this, &ABasePlayerController::TurnAtRate);
    InputComponent->BindAxis("LookUpAtRate", this, &ABasePlayerController::LookUpAtRate);
    InputComponent->BindAxis("MoveForward", this, &ABasePlayerController::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &ABasePlayerController::MoveRight);

    InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &ABasePlayerController::Mantle);
    InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ABasePlayerController::Jump);
    InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartSprint);
    InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &ABasePlayerController::StopSprint);

    InputComponent->BindAxis("SwimForward", this, &ABasePlayerController::SwimForward);
    InputComponent->BindAxis("SwimRight", this, &ABasePlayerController::SwimRight);
    InputComponent->BindAxis("SwimUp", this, &ABasePlayerController ::SwimUp);

    InputComponent->BindAxis("Climb", this, &ABasePlayerController::Climb);

    InputComponent->BindAction("Interact", IE_Pressed, this, &ABasePlayerController::Interact);

    InputComponent->BindAction("Slide", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartSliding);
    InputComponent->BindAction("Slide", EInputEvent::IE_Released, this, &ABasePlayerController::StopSliding);
    InputComponent->BindAction("Prone", IE_Pressed, this, &ABasePlayerController::ToggleProneState);
    InputComponent->BindAction("Crouch", IE_Pressed, this, &ABasePlayerController::ToggleCrouchState);

    InputComponent->BindAction("Wallrun", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartWallrun);
    InputComponent->BindAction("Wallrun", EInputEvent::IE_Released, this, &ABasePlayerController::StopWallrun);

    InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartFiring);
    InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ABasePlayerController::StopFiring);

    InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartAiming);
    InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &ABasePlayerController::StopAiming);

    InputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &ABasePlayerController::Reload);

    InputComponent->BindAction("NextWeapon", EInputEvent::IE_Pressed, this, &ABasePlayerController::PickNextWeapon);
    InputComponent->BindAction("PreviousWeapon", EInputEvent::IE_Pressed, this, &ABasePlayerController::PickPreviousWeapon);

    InputComponent->BindAction("ThrowItem", EInputEvent::IE_Pressed, this, &ABasePlayerController::ThrowItem);

    InputComponent->BindAction("PrimaryMeleeAttack", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartPrimaryMeleeAttack);
    InputComponent->BindAction("PrimaryMeleeAttack", EInputEvent::IE_Released, this, &ABasePlayerController::StopPrimaryMeleeAttack);

    InputComponent->BindAction("SecondaryMeleeAttack", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartSecondaryMeleeAttack);
    InputComponent->BindAction("SecondaryMeleeAttack", EInputEvent::IE_Released, this, &ABasePlayerController::StopSecondaryMeleeAttack);

    InputComponent->BindAction("ToggleFireMode", EInputEvent::IE_Pressed, this, &ABasePlayerController::ToggleFireMode);

    InputComponent->BindAction("PauseGame", EInputEvent::IE_Pressed, this, &ABasePlayerController::OnPauseGame);
    InputComponent->BindAction("QuickSave", EInputEvent::IE_Pressed, this, &ABasePlayerController::SaveGame);
    InputComponent->BindAction("QuickLoad", EInputEvent::IE_Pressed, this, &ABasePlayerController::LoadGame);

    InputComponent->BindAction("SkipCutscene", EInputEvent::IE_Pressed, this, &ABasePlayerController::SkipCutscene);
    InputComponent->BindAction("NextPage", EInputEvent::IE_Pressed, this, &ABasePlayerController::ShowNextPage);
    InputComponent->BindAction("PreviousPage", EInputEvent::IE_Pressed, this, &ABasePlayerController::ShowPreviousPage);
    InputComponent->BindAction("CloseReadable", EInputEvent::IE_Pressed, this, &ABasePlayerController::CloseReadable);
    InputComponent->BindAction("JournalOpen", EInputEvent::IE_Pressed, this, &ABasePlayerController::OnJournalOpen);
    
    InputComponent->BindAction("SkipDialogue", EInputEvent::IE_Pressed, this, &ABasePlayerController::SkipDialogueLine);
    
    InputComponent->BindAction("UnequipWeapon", IE_Pressed, this, &ABasePlayerController::UnequipWeapon);
    InputComponent->BindAction("EquipPrimaryWeapon", IE_Pressed, this, &ABasePlayerController::EquipPrimaryWeapon);
    InputComponent->BindAction("EquipSecondaryWeapon", IE_Pressed, this, &ABasePlayerController::EquipSecondaryWeapon);
    InputComponent->BindAction("EquipMeleeWeapon", IE_Pressed, this, &ABasePlayerController::EquipMeleeWeapon);
    InputComponent->BindAction("EquipPrimaryThrowable", IE_Pressed, this, &ABasePlayerController::EquipPrimaryThrowable);
    InputComponent->BindAction("EquipSecondaryThrowable", IE_Pressed, this, &ABasePlayerController::EquipSecondaryThrowable);

    InputComponent->BindAction("ToggleWalking", IE_Pressed, this, &ABasePlayerController::ToggleWalking);
}

#pragma region CLIMBING

void ABasePlayerController::Climb(float Value)
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->Climb(Value);
    }
}

#pragma endregion CLIMBING

#pragma region SWIMMING

void ABasePlayerController::SwimUp(float Value)
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->SwimUp(Value);
    }
}

void ABasePlayerController::SwimRight(float Value)
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->SwimRight(Value);
    }
}

void ABasePlayerController::SwimForward(float Value)
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->SwimForward(Value);
    }
}

#pragma endregion SWIMMING

#pragma region VIEW

void ABasePlayerController::TurnAtRate(float Value)
{
    if (IsValid(PlayerCharacter) && PlayerGameState == EPlayerGameState::Game)
    {
        PlayerCharacter->TurnAtRate(Value);
    }
}

void ABasePlayerController::LookUpAtRate(float Value)
{
    if (IsValid(PlayerCharacter) && PlayerGameState == EPlayerGameState::Game)
    {
        PlayerCharacter->LookUpAtRate(Value);
    }
}

void ABasePlayerController::LookUp(float Value)
{
    if (IsValid(PlayerCharacter) && PlayerGameState == EPlayerGameState::Game)
    {
        PlayerCharacter->LookUp(Value);
    }
}

void ABasePlayerController::Turn(float Value)
{
    if (IsValid(PlayerCharacter) && PlayerGameState == EPlayerGameState::Game)
    {
        PlayerCharacter->Turn(Value);
    }
}

#pragma endregion VIEW

#pragma region MOVEMENT

void ABasePlayerController::MoveForward(float Value)
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->MoveForward(Value);
    }
}

void ABasePlayerController::MoveRight(float Value)
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->MoveRight(Value);
    }
}

void ABasePlayerController::Mantle()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->Mantle();
    }
}

void ABasePlayerController::Jump()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->Jump();
    }
}

void ABasePlayerController::StartSliding()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->TryStartSliding();
    }
}

void ABasePlayerController::StopSliding()
{
    if (IsValid(PlayerCharacter))
    {
        PlayerCharacter->StopSliding();
    }
}

void ABasePlayerController::ToggleCrouchState()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->ToggleCrouchState();
    }
}

void ABasePlayerController::ToggleProneState()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->ToggleProneState();
    }
}

void ABasePlayerController::StartSprint()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->StartRequestingSprint();
    }
}

void ABasePlayerController::StopSprint()
{
    if (IsValid(PlayerCharacter))
    {
        PlayerCharacter->StopRequestingSprint();
    }
}

void ABasePlayerController::StartWallrun()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->StartRequestingWallrun();
    }
}

void ABasePlayerController::StopWallrun()
{
    if (IsValid(PlayerCharacter))
    {
        PlayerCharacter->StopRequestingWallrun();
    }
}

#pragma endregion MOVEMENT

#pragma region ACTIONS

void ABasePlayerController::Interact()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->Interact();
    }
}

void ABasePlayerController::StartFiring()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->StartFiring();
    }
}

void ABasePlayerController::StartAiming()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->StartAiming();
    }
}

void ABasePlayerController::StopAiming()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->StopAiming();
    }
}

void ABasePlayerController::Reload()
{
    if (IsValid(PlayerCharacter))
    {
        PlayerCharacter->StartReloading();
    }
}

void ABasePlayerController::PickNextWeapon()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->ChangeWeapon(1);
    }
}

void ABasePlayerController::PickPreviousWeapon()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->ChangeWeapon(-1);
    }
}

void ABasePlayerController::ThrowItem()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->ThrowItem();
    }
}

void ABasePlayerController::StartPrimaryMeleeAttack()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->StartPrimaryMeleeAttack();
    }
}

void ABasePlayerController::StopPrimaryMeleeAttack()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->StopPrimaryMeleeAttack();
    }
}

void ABasePlayerController::StartSecondaryMeleeAttack()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->StartSecondaryMeleeAttack();
    }
}

void ABasePlayerController::StopSecondaryMeleeAttack()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->StopSecondaryMeleeAttack();
    }
}

void ABasePlayerController::ToggleFireMode()
{
    if (IsValid(PlayerCharacter) && bCharacterInputEnabled)
    {
        PlayerCharacter->ToggleFireMode();
    }
}

void ABasePlayerController::StopFiring()
{
    if (IsValid(PlayerCharacter))
    {
        PlayerCharacter->StopFiring();
    }
}

void ABasePlayerController::UnequipWeapon()
{
    if (IsValid(PlayerCharacter))
        PlayerCharacter->GetEquipmentComponent()->UnequipItem();
}

void ABasePlayerController::EquipPrimaryWeapon()
{
    if (IsValid(PlayerCharacter))
        PlayerCharacter->GetEquipmentComponent()->EquipPrimaryWeapon();
}

void ABasePlayerController::EquipSecondaryWeapon()
{
    if (IsValid(PlayerCharacter))
        PlayerCharacter->GetEquipmentComponent()->EquipSecondaryWeapon();
}

void ABasePlayerController::EquipMeleeWeapon()
{
    if (IsValid(PlayerCharacter))
        PlayerCharacter->GetEquipmentComponent()->EquipMeleeWeapon();
}

void ABasePlayerController::EquipPrimaryThrowable()
{
    if (IsValid(PlayerCharacter))
        PlayerCharacter->GetEquipmentComponent()->EquipPrimaryThrowable();
}

void ABasePlayerController::EquipSecondaryThrowable()
{
    if (IsValid(PlayerCharacter))
        PlayerCharacter->GetEquipmentComponent()->EquipSecondaryThrowable();
}

void ABasePlayerController::ToggleWalking()
{
    if (IsValid(PlayerCharacter))
        PlayerCharacter->GetHumanoidMovementComponent()->ToggleWalking();
}

#pragma endregion ACTIONS

#pragma region READABLES

void ABasePlayerController::CloseReadable()
{
    if (PlayerGameState == EPlayerGameState::Reading)
    {
        PlayerHUDWidget->CloseReadable();
        bCharacterInputEnabled = true;
        PlayerGameState = EPlayerGameState::Game;
    }
}

void ABasePlayerController::ReadItem(const FDataTableRowHandle& ReadableDTRH)
{
    PlayerHUDWidget->TryOpenReadable(ReadableDTRH);
    bCharacterInputEnabled = false;
    PlayerGameState = EPlayerGameState::Reading;
    auto QuestSubsystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>();
    QuestSubsystem->OnReadableRead(ReadableDTRH);
}

void ABasePlayerController::ShowNextPage()
{
    if (PlayerGameState == EPlayerGameState::Reading)
        PlayerHUDWidget->ReadableNextPage();
}

void ABasePlayerController::ShowPreviousPage()
{
    if (PlayerGameState == EPlayerGameState::Reading)
        PlayerHUDWidget->ReadablePreviousPage();
}

#pragma endregion READABLES

#pragma region DIALOGUES

void ABasePlayerController::ShowNpcSpeechSubtitles(const FText& NpcName, const FText& SpeechText,
    const FVector& NpcLocation, float SpeechLoudness) const
{
    if (!PlayerCharacter)
        return;
    
    float NpcToPlayerSqDistance = FVector::DistSquared(PlayerCharacter->GetActorLocation(), NpcLocation);
    if (NpcToPlayerSqDistance > MaxNpcSubtitlingRange * MaxNpcSubtitlingRange * SpeechLoudness)
        return;

    PlayerHUDWidget->AddSubtitleLine(NpcName, SpeechText, 3.f);
}

void ABasePlayerController::OnDialogueStarted(bool bLocking, const FVector& NpcLocation)
{
    if (bLocking)
    {
        bCharacterInputEnabled = false;
        PlayerGameState = EPlayerGameState::Dialogue;
        PlayerCharacter->SetDesiredRotation((NpcLocation - PlayerCharacter->GetActorLocation()).Rotation());
    }

    PlayerHUDWidget->StartDialogue();
}

void ABasePlayerController::SkipDialogueLine()
{
    if (!bCharacterInputEnabled && PlayerGameState == EPlayerGameState::Dialogue)
    {
        auto DS = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
        DS->SkipLine();
    }
}

void ABasePlayerController::OnDialogueEnded(const TArray<FDialogueParticipant>& Participants, bool bWasLocking, bool bCompleted)
{
    // TODO now there are 2+ sources of locking input. bCharacterInputEnabled should be transformed into int
    if (bWasLocking)
        bCharacterInputEnabled = true;
    
    PlayerGameState = EPlayerGameState::Game;

    if (!bCompleted)
        return;

    PlayerHUDWidget->EndDialogue();
}

#pragma endregion DIALOGUES

#pragma region GAMECONTROL

void ABasePlayerController::SkipCutscene()
{
    auto QuestSubsystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>();
    QuestSubsystem->TrySkipCutscene();
}

void ABasePlayerController::OnPauseGame()
{
    if (!GetWorld() || !GetWorld()->GetAuthGameMode())
        return;

    // PlayerGameState = EPlayerGameState::Pause; ??
    // bCharacterInputEnabled = false;
    GetWorld()->GetAuthGameMode()->SetPause(this);
}

void ABasePlayerController::OnJournalOpen()
{
    if (!GetWorld() || !GetWorld()->GetAuthGameMode())
        return;

    const auto GameMode = Cast<AMLGameModeBase>(GetWorld()->GetAuthGameMode());
    GameMode->OpenJournal(this);
}

void ABasePlayerController::SaveGame()
{
    USaveGameSubsystem* SaveGameInstance = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<USaveGameSubsystem>();
    if (SaveGameInstance != nullptr)
    {
        SaveGameInstance->QuickSave();
    }
}

void ABasePlayerController::LoadGame()
{
    USaveGameSubsystem* SaveGameInstance = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
    if (SaveGameInstance != nullptr)
        SaveGameInstance->LoadLastGame();
}

#pragma endregion GAMECONTROL

#pragma region EVENT HANDLERS

void ABasePlayerController::OnAttributeChanged(ECharacterAttribute Attribute, float Percent) const
{
    if (IsValid(PlayerHUDWidget))
    {
        PlayerHUDWidget->OnAttributeChanged(Attribute, Percent);
    }
}

void ABasePlayerController::OnAimingStateChanged(bool bAiming, ARangeWeaponItem* Weapon)
{
    if (IsValid(PlayerHUDWidget))
    {
        PlayerHUDWidget->OnAimingStateChanged(bAiming, IsValid(Weapon) ? Weapon->GetReticleType() : EReticleType::None);
    }

    if (IsValid(Weapon) && Weapon->GetAimReticleType() == EReticleType::Scope)
    {
        FViewTargetTransitionParams TransitionParams;
        TransitionParams.BlendTime = 0.2;
        if (bAiming)
        {
            SetViewTarget(Weapon, TransitionParams);
        }
        else
        {
            SetViewTarget(PlayerCharacter, TransitionParams);
        }
    }
    else if (!IsValid(Weapon) && !bAiming && GetViewTarget() != PlayerCharacter)
    {
        SetViewTarget(PlayerCharacter);
    }
}

void ABasePlayerController::OnAmmoChanged(int32 NewAmmo, int32 TotalAmmo)
{
    if (IsValid(PlayerHUDWidget))
    {
        PlayerHUDWidget->SetAmmo(NewAmmo, TotalAmmo);
    }
}

void ABasePlayerController::OnThrowableEquipped(AThrowableItem* Throwable, int32 Count)
{
    if (IsValid(PlayerHUDWidget))
    {
        PlayerHUDWidget->SetThrowableName(Throwable->GetName());
        PlayerHUDWidget->SetThrowablesCount(Count);
        UpdateWeaponInfoVisibility();
    }
}

void ABasePlayerController::OnThowablesCountChanged(int32 Count)
{
    if (IsValid(PlayerHUDWidget))
    {
        PlayerHUDWidget->SetThrowablesCount(Count);
        UpdateWeaponInfoVisibility();
    }
}

void ABasePlayerController::OnWeaponEquipped(const FText& Name, EReticleType Reticle)
{
    if (IsValid(PlayerHUDWidget))
    {
        PlayerHUDWidget->SetReticleType(Reticle);
        PlayerHUDWidget->SetWeaponName(Name);
        UpdateWeaponInfoVisibility();
    }
}

void ABasePlayerController::OnWeaponAmmoInfoChanged(bool bInfiniteClip, bool bInfiniteAmmoSupply)
{
    if (IsValid(PlayerHUDWidget))
        PlayerHUDWidget->SetAmmoInfo(bInfiniteClip, bInfiniteAmmoSupply);
}

void ABasePlayerController::OnWeaponUnequipped()
{
    if (IsValid(PlayerHUDWidget))
    {
        PlayerHUDWidget->OnWeaponUnequipped();
        UpdateWeaponInfoVisibility();
    }
}

void ABasePlayerController::UpdateWeaponInfoVisibility()
{
    auto EquipmentComponent = PlayerCharacter->GetEquipmentComponent();
    PlayerHUDWidget->SetWeaponInfoVisible(EquipmentComponent->IsWeaponEquipped(),EquipmentComponent->IsThrowableEquipped());
}

void ABasePlayerController::OnMeleeWeaponEquipped()
{
    if (IsValid(PlayerHUDWidget))
    {
        PlayerHUDWidget->OnMeleeWeaponEquipped();
        UpdateWeaponInfoVisibility();
    }
}

void ABasePlayerController::OnGameStateChanged(EMLGameState State)
{
    if (State == EMLGameState::InProgress)
    {
        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = false;
    }
    else
    {
        SetInputMode(FInputModeUIOnly());
        bShowMouseCursor = true;
    }
}

void ABasePlayerController::OnQuestStarted(const FQuestDTR* QuestDTR)
{
    if (QuestDTR->InitialDialogue.DialogueScenarios.Num() > 0)
    {
        auto DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
        DialogueSubsystem->StartAutonomousDialogue(QuestDTR->InitialDialogue);
    }
}

void ABasePlayerController::OnQuestEventOccured(const FQuestTaskDTR* QuestTaskDTR) 
{
    if (QuestTaskDTR->Dialogue.DialogueScenarios.Num() > 0)
    {
        auto DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
        TArray<ABaseCharacter*> Participants;
        Participants.Add(PlayerCharacter);
        DialogueSubsystem->StartAutonomousDialogue(QuestTaskDTR->Dialogue);
    }
}

void ABasePlayerController::OnQuestCompleted(const FQuestDTR* QuestDTR, bool bAutocompleted)
{
    if (QuestDTR->EndDialogue.DialogueScenarios.Num() > 0)
    {
        auto DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
        DialogueSubsystem->StartAutonomousDialogue(QuestDTR->EndDialogue);
    }
}

#pragma endregion EVENT HANDLERS

void ABasePlayerController::SetInputLocked(bool bInputLocked)
{
    bCharacterInputEnabled = !bInputLocked;
    SetIgnoreLookInput(bInputLocked);
    SetIgnoreMoveInput(bInputLocked);
}

void ABasePlayerController::OnCutsceneStarted()
{
    SetInputLocked(true);
    PlayerGameState = EPlayerGameState::Cutscene;
    PlayerHUDWidget->SetVisibility(ESlateVisibility::Hidden);
    PlayerCharacter->ChangeGameplayTags(CutsceneGameplayTag.GetSingleTagContainer());
}

void ABasePlayerController::OnCutsceneEnded()
{
    SetInputLocked(false);
    PlayerGameState = EPlayerGameState::Game;
    PlayerHUDWidget->SetVisibility(ESlateVisibility::Visible);
    PlayerCharacter->ChangeGameplayTags(CutsceneGameplayTag.GetSingleTagContainer(), false);
}

void ABasePlayerController::ReportKill(const AActor* KilledActor)
{
    auto QuestSubsystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>();
    QuestSubsystem->OnNpcKilled(KilledActor);
}

void ABasePlayerController::RegisterObservationSource(UObservationSourceComponent* ObservationSourceComponent)
{
    ObservationSources.AddUnique(ObservationSourceComponent);
}

void ABasePlayerController::UpdateObservationSources()
{
    TArray<UObservationSourceComponent*> ItemsToRemove;
    for (auto ObservationSource : ObservationSources)
    {
        if (IsValid(ObservationSource) && ObservationSource->IsSightSourceActive())
            ObservationSource->UpdateVisibility();
        else
            ItemsToRemove.Add(ObservationSource);
    }

    for (auto InvalidObservationSource : ItemsToRemove)
        ObservationSources.RemoveSingleSwap(InvalidObservationSource);
}

void ABasePlayerController::OnItemPickedUp(const FDataTableRowHandle& ItemDTRH)
{
    auto QuestSystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>();
    QuestSystem->OnItemPickedUp(PlayerCharacter->GetInventoryComponent(), ItemDTRH);
}

