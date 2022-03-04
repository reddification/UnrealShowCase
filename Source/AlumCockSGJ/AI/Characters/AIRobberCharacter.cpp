// Fill out your copyright notice in the Description page of Project Settings.

#include "AIRobberCharacter.h"
#include "Components/WidgetComponent.h"
#include "UI/CharacterInfoWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"
#include "AI/Data/NpcDTR.h"
#include "Subsystems/NpcSubsystem.h"


AAIRobberCharacter::AAIRobberCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

    CharacterInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("CharacterInfoWidget");
    CharacterInfoWidgetComponent->SetupAttachment(GetMesh());
    CharacterInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    CharacterInfoWidgetComponent->SetDrawAtDesiredSize(true);

    bUseControllerRotationYaw = false;
}

void AAIRobberCharacter::BeginPlay()
{
    Super::BeginPlay();
    check(CharacterInfoWidgetComponent);
    CharacterAttributesComponent->AttributeChangedEvent.AddUObject(this, &AAIRobberCharacter::OnAttributeChanged);
    CharacterInfoWidgetComponent->SetVisibility(false);
    CharacterInfoWidget = Cast<UCharacterInfoWidget>(CharacterInfoWidgetComponent->GetWidget());
    if (CharacterInfoWidget.IsValid())
    {
        CharacterInfoWidget->SetName(CharacterSettings->Name);
    }
}

void AAIRobberCharacter::OnAttributeChanged(ECharacterAttribute Attribute, float Percent) const
{
    if (Attribute == ECharacterAttribute::Health && HealthChangedEvent.IsBound())
    {
        HealthChangedEvent.Broadcast();
    }
}

void AAIRobberCharacter::ReactToDamage(
    AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    Super::ReactToDamage(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);
    if (!CharacterAttributesComponent->IsAlive())
    {
        CharacterInfoWidgetComponent->SetVisibility(false);
        return;
    }

    UAISense_Damage::ReportDamageEvent(GetWorld(), this, DamageCauser, Damage, DamageCauser->GetActorLocation(), GetActorLocation());
}

float AAIRobberCharacter::GetHealth()
{
    return CharacterAttributesComponent->GetHealth();
}

void AAIRobberCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (CharacterInfoWidgetComponent->IsVisible())
    {
        auto NpcSubsystem = GetWorld()->GetSubsystem<UNpcSubsystem>();
        FRotator NewRotation = NpcSubsystem->GetFacePlayerViewpointRotation(CharacterInfoWidgetComponent->GetComponentLocation());
        CharacterInfoWidgetComponent->SetWorldRotation(NewRotation);
    }
}

void AAIRobberCharacter::SetNpcDTRH(const FDataTableRowHandle& InNpcDTRH)
{
    NpcDTRH = InNpcDTRH;
    InitializeNPC();
}

void AAIRobberCharacter::InitializeNPC_Implementation()
{
    if (NpcDTRH.IsNull())
        return;

    FNpcDTR* NpcDTR = NpcDTRH.DataTable->FindRow<FNpcDTR>(NpcDTRH.RowName, "");
    if (!NpcDTR)
        return;

    CharacterInfoWidget = Cast<UCharacterInfoWidget>(CharacterInfoWidgetComponent->GetWidget());
    if (CharacterInfoWidget.IsValid())
        CharacterInfoWidget->SetName(NpcDTR->Name);
}