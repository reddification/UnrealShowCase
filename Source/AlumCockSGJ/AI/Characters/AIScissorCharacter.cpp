// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Characters/AIScissorCharacter.h"
#include "Components/WidgetComponent.h"
#include "UI/CharacterInfoWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"
#include "Subsystems/NpcSubsystem.h"


AAIScissorCharacter::AAIScissorCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    CharacterInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("CharacterInfoWidget");
    CharacterInfoWidgetComponent->SetupAttachment(GetMesh());
    CharacterInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    CharacterInfoWidgetComponent->SetDrawAtDesiredSize(true);

    bUseControllerRotationYaw = false;
}

void AAIScissorCharacter::BeginPlay()
{
    Super::BeginPlay();
    check(CharacterInfoWidgetComponent);
    CharacterAttributesComponent->AttributeChangedEvent.AddUObject(this, &AAIScissorCharacter::OnAttributeChanged);
    CharacterInfoWidgetComponent->SetVisibility(false);
    CharacterInfoWidget = Cast<UCharacterInfoWidget>(CharacterInfoWidgetComponent->GetWidget());
    if (CharacterInfoWidget.IsValid())
    {
        CharacterInfoWidget->SetName(CharacterSettings->Name);
    }
}

void AAIScissorCharacter::OnAttributeChanged(ECharacterAttribute Attribute, float Percent) const
{
    if (Attribute == ECharacterAttribute::Health && HealthChangedEvent.IsBound())
    {
        HealthChangedEvent.Broadcast();
    }
}

void AAIScissorCharacter::ReactToDamage(
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

float AAIScissorCharacter::GetHealth()
{
    return CharacterAttributesComponent->GetHealth();
}

void AAIScissorCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (CharacterInfoWidgetComponent->IsVisible())
    {
        auto NpcSubsystem = GetWorld()->GetSubsystem<UNpcSubsystem>();
        FRotator NewRotation = NpcSubsystem->GetFacePlayerViewpointRotation(CharacterInfoWidgetComponent->GetComponentLocation());
        CharacterInfoWidgetComponent->SetWorldRotation(NewRotation);
    }
}