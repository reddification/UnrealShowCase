// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/Interactable.h"
#include "AI/Interfaces/BasicActivityCharacter.h"
#include "AIScissorCharacter.generated.h"

/**
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHealthChangedEvent);

class UWidgetComponent;

UCLASS()
class ALUMCOCKSGJ_API AAIScissorCharacter : public ABaseCharacter, public IInteractable, public IBasicActivityCharacter
{
    GENERATED_BODY()
public:
    AAIScissorCharacter(const FObjectInitializer& ObjectInitializer);
    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UWidgetComponent* CharacterInfoWidgetComponent;

    virtual void ReactToDamage(
        AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) override;

private:
    TWeakObjectPtr<class UCharacterInfoWidget> CharacterInfoWidget;
    void OnAttributeChanged(ECharacterAttribute Attribute, float Percent) const;

public:
    UPROPERTY(BlueprintAssignable)
    FHealthChangedEvent HealthChangedEvent;
    UFUNCTION(BlueprintCallable)
    float GetHealth();
};
