// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Interfaces/Interactable.h"
#include "Engine/DataTable.h"
#include "AIRobberCharacter.generated.h"

/**
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRobberHealthChangedEvent);

class UWidgetComponent;

UCLASS()
class ALUMCOCKSGJ_API AAIRobberCharacter : public ABaseHumanoidCharacter, public IInteractable
{
    GENERATED_BODY()
public:
    AAIRobberCharacter(const FObjectInitializer& ObjectInitializer);
    virtual void Tick(float DeltaSeconds) override;

    virtual void SetNpcDTRH(const FDataTableRowHandle& NpcDTRH);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UWidgetComponent* CharacterInfoWidgetComponent;

    virtual void ReactToDamage(
        AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FDataTableRowHandle NpcDTRH;

    
	UFUNCTION(BlueprintNativeEvent)
    void InitializeNPC();
    virtual void InitializeNPC_Implementation();

private:
    TWeakObjectPtr<class UCharacterInfoWidget> CharacterInfoWidget;
    void OnAttributeChanged(ECharacterAttribute Attribute, float Percent) const;
    
public:
    UPROPERTY(BlueprintAssignable)
    FRobberHealthChangedEvent HealthChangedEvent;
    UFUNCTION(BlueprintCallable)
    float GetHealth();
};