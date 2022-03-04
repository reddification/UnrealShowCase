#pragma once

#include "CoreMinimal.h"
#include "AI/Interfaces/BasicActivityCharacter.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Interfaces/Interactable.h"
#include "AICharacter.generated.h"

class UAIPatrolComponent;
class UBehaviorTree;
class UAICharacterMovementComponent;
class UWidgetComponent;

UCLASS(Blueprintable)
class ALUMCOCKSGJ_API AAICharacter : public ABaseCharacter, public IInteractable, public IBasicActivityCharacter
{
    GENERATED_BODY()

public:
    AAICharacter(const FObjectInitializer& ObjectInitializer);

    UAIPatrolComponent* GetAIPatrolComponent() const { return AIPatrolComponent; }
    virtual UBehaviorTree* GetDefaultBehaviorTree() const override { return DefaultBehaviorTree; }

    virtual void PossessedBy(AController* NewController) override;
    virtual void Tick(float DeltaSeconds) override;

    virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
    
    class UAICharacterMovementComponent* GetAIMovementComponent() const { return AIMovementComponent.Get(); }
    
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UAIPatrolComponent* AIPatrolComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UBehaviorTree* DefaultBehaviorTree;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UWidgetComponent* CharacterInfoWidgetComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UObservationSourceComponent* ObservationSourceComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTagContainer GameplayTags;
    
    virtual void ReactToDamage(
        AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) override;

    virtual void BeginPlay() override;

private:
    TWeakObjectPtr<UAICharacterMovementComponent> AIMovementComponent;
    TWeakObjectPtr<class UCharacterInfoWidget> CharacterInfoWidget;
    
    void OnAttributeChanged(ECharacterAttribute Attribute, float Percent) const;
    
    void OnSightStateChanged(bool bInSight);
    int InSightCounter = 0;
};

