#include "BaseWorldItem.h"
//#include "Components/BillboardComponent.h"
#include "Components/WidgetComponent.h"
#include "Data/Entities/WorldItemDTR.h"
#include "UI/BaseInteractableInfoWidget.h"

ABaseWorldItem::ABaseWorldItem()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComponent);
	
	/* BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	BillboardComponent->SetupAttachment(RootComponent);
      */
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	ItemInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	ItemInfoWidgetComponent->SetupAttachment(StaticMeshComponent);
}

void ABaseWorldItem::BeginPlay()
{
	Super::BeginPlay();
	InitializeWorldItem();
	ItemInfoWidgetComponent->SetVisibility(false);
}

void ABaseWorldItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	InitializeWorldItem();
}

void ABaseWorldItem::InitializeWorldItem()
{
	if (ItemDTRH.IsNull() || ItemDTRH.RowName.IsNone())
		return;
	
	FWorldItemDTR* WorldItemDTR = ItemDTRH.DataTable->FindRow<FWorldItemDTR>(ItemDTRH.RowName, "");
	if (!WorldItemDTR)
		return;
	
	auto ItemInfoWidget = Cast<UBaseInteractableInfoWidget>(ItemInfoWidgetComponent->GetWidget());
	if (IsValid(ItemInfoWidget))
		ItemInfoWidget->SetName(WorldItemDTR->Name);
}

void ABaseWorldItem::SetItemDTRH(const FDataTableRowHandle& NewItemDTRH)
{
	ItemDTRH = NewItemDTRH;
	InitializeWorldItem();
}

void ABaseWorldItem::OutOfSight()
{
	ItemInfoWidgetComponent->SetVisibility(false);
}

void ABaseWorldItem::ChangeGameplayTags(const FGameplayTagContainer& TagsContainer, bool bAdd = true)
{
	if (bAdd)
		GameplayTags.AppendTags(TagsContainer);
	else
		GameplayTags.RemoveTags(TagsContainer);
}

void ABaseWorldItem::InSight()
{
	ItemInfoWidgetComponent->SetVisibility(true);
}
