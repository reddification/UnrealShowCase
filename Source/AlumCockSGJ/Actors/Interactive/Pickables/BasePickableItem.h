#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/BaseWorldItem.h"
#include "Components/BillboardComponent.h"
#include "Engine/DataTable.h"
#include "Interfaces/Interactable.h"
#include "BasePickableItem.generated.h"

struct FWorldItemDTR;
UCLASS()
class ALUMCOCKSGJ_API ABasePickableItem : public ABaseWorldItem
{
    GENERATED_BODY()

public:
    ABasePickableItem();
    
protected:
    virtual void BeginPlay() override;
    virtual void InitializeWorldItem() override;
    
    virtual bool PickUp(class APlayerCharacter* PlayerCharacter, const FWorldItemDTR* WorldItemDTR, bool bForce = false);
    void PlayPickUpSound(const FWorldItemDTR* WorldItemDTR);

private:
    UFUNCTION()
    void OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);
};
