#include "CarryingComponent.h"

#include "Characters/BaseHumanoidCharacter.h"
#include "Data/DataAssets/Components/CarryingSettings.h"
#include "Interfaces/CarriableObject.h"

void UCarryingComponent::BeginPlay()
{
	Super::BeginPlay();
	HumanoidCharacter = Cast<ABaseHumanoidCharacter>(GetOwner());
}

bool UCarryingComponent::StartCarrying(AActor* Item)
{
	if (bCarrying || bInteractionInProcess || !HumanoidCharacter->CanStartAction(ECharacterAction::CarryObject) || !Item->Implements<UCarriableObject>())
		return false;

	CarriedItem = Item;
	bInteractionInProcess = true;
	if (CarryingSettings->PickUpMontage)
		HumanoidCharacter->PlayAnimMontageWithDuration(CarryingSettings->PickUpMontage, CarryingSettings->PickUpDurationSeconds);
	
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &UCarryingComponent::FinishPickingUp, CarryingSettings->PickUpDurationSeconds);

	bInteractionInProcess = true;
	HumanoidCharacter->OnActionStarted(ECharacterAction::CarryObject);
	return true;
}

bool UCarryingComponent::StopCarrying()
{
	if (!bCarrying || bInteractionInProcess)
		return false;

	bInteractionInProcess = true;
	if (CarryingSettings->ReleaseMontage)
		HumanoidCharacter->PlayAnimMontageWithDuration(CarryingSettings->ReleaseMontage, CarryingSettings->ReleaseDurationSeconds);
	
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &UCarryingComponent::ReleaseItem, CarryingSettings->ReleaseDurationSeconds);
	return true;
}

bool UCarryingComponent::StartPutAtLocation(FVector Location)
{
	if (!bCarrying || bInteractionInProcess || !IsValid(CarriedItem))
		return false;

	bInteractionInProcess = true;
	LocationToPutActor = Location;
	if (CarryingSettings->PutMontage)
		HumanoidCharacter->PlayAnimMontageWithDuration(CarryingSettings->PutMontage, CarryingSettings->PutDurationSeconds);
	
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &UCarryingComponent::FinishPutInLocation, CarryingSettings->PutDurationSeconds);
	return true;
}

bool UCarryingComponent::InterruptCarrying()
{
	if (!bCarrying)
		return false;

	GetWorld()->GetTimerManager().ClearTimer(Timer);
	FinishReleasingItem();
	ResetCarryingState(ECarryingState::Interruped);
	return true;
}

void UCarryingComponent::ReleaseItem()
{
	bInteractionInProcess = false;
	FinishReleasingItem();
	ResetCarryingState(ECarryingState::Stopped);
}

void UCarryingComponent::AttachItemToCharacter()
{
	if (bAttached)
		return;
	
	ICarriableObject::Execute_OnCarriedStart(CarriedItem);
	CarryingType = ICarriableObject::Execute_GetCarryingObjectType(CarriedItem);
	const FName AttachmentSocket = ICarriableObject::Execute_GetCharacterAttachmentSocket(CarriedItem);
	CarriedItem->AttachToComponent(HumanoidCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachmentSocket);
	bAttached = true;
}

void UCarryingComponent::FinishPickingUp()
{
	if (!bInteractionInProcess)
		return;

	bInteractionInProcess = false;
	if (!IsValid(CarriedItem))
	{
		ResetCarryingState(ECarryingState::Interruped);
		return;
	}
	
	bCarrying = true;
	
	AttachItemToCharacter();

	if (CarryingStateChangedEvent.IsBound())
		CarryingStateChangedEvent.Broadcast(HumanoidCharacter, ECarryingState::Started);
}

void UCarryingComponent::DetachItemFromCharacter()
{
	if (!bAttached)
		return;
	
	CarriedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ICarriableObject::Execute_OnCarriedStop(CarriedItem);
	bAttached = false;
}

void UCarryingComponent::FinishReleasingItem()
{
	if (!IsValid(CarriedItem))
		return;
	
	DetachItemFromCharacter();
}

void UCarryingComponent::FinishPutInLocation()
{
	FinishReleasingItem();	
	CarriedItem->SetActorLocation(LocationToPutActor);
	ResetCarryingState(ECarryingState::Stopped);
}

void UCarryingComponent::ResetCarryingState(ECarryingState Reason)
{
	CarriedItem = nullptr;
	bInteractionInProcess = false;
	CarryingType = ECarryingType::None;
	bCarrying = false;
	LocationToPutActor = FVector::ZeroVector;
	HumanoidCharacter->OnActionEnded(ECharacterAction::CarryObject);
	if (CarryingStateChangedEvent.IsBound())
		CarryingStateChangedEvent.Broadcast(HumanoidCharacter, Reason);
}
