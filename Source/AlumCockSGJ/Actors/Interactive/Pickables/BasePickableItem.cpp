#include "BasePickableItem.h"

#include "Characters/PlayerCharacter.h"
#include "Components/BillboardComponent.h"
#include "Data/Entities/WorldItemDTR.h"
#include "Kismet/GameplayStatics.h"

ABasePickableItem::ABasePickableItem()
{
	StaticMeshComponent->SetCollisionProfileName("OverlapOnlyPawn");
}

void ABasePickableItem::BeginPlay()
{
	Super::BeginPlay();
	StaticMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ABasePickableItem::OnOverlapped);
}

// TODO separate method PlayPickupSound?
bool ABasePickableItem::PickUp(APlayerCharacter* PlayerCharacter, const FWorldItemDTR* WorldItemDTR, bool bForce)
{
	return true;
}

void ABasePickableItem::PlayPickUpSound(const FWorldItemDTR* WorldItemDTR)
{
	if (IsValid(WorldItemDTR->PickupSFX))
		UGameplayStatics::PlaySound2D(GetWorld(), WorldItemDTR->PickupSFX);
}

void ABasePickableItem::OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ItemDTRH.IsNull())
		return;
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (!IsValid(PlayerCharacter))
		return;

	FWorldItemDTR* WorldItemDTR = ItemDTRH.DataTable->FindRow<FWorldItemDTR>(ItemDTRH.RowName, "");
	if (WorldItemDTR == nullptr)
		return;
	
	PickUp(PlayerCharacter, WorldItemDTR, false);
}

void ABasePickableItem::InitializeWorldItem()
{
	Super::InitializeWorldItem();
	if (!ItemDTRH.IsNull())
	{
		FWorldItemDTR* WorldItemData = ItemDTRH.DataTable->FindRow<FWorldItemDTR>(ItemDTRH.RowName, "");
		if (WorldItemData != nullptr && IsValid(StaticMeshComponent))
			StaticMeshComponent->SetStaticMesh(WorldItemData->StaticMesh);
	}
}