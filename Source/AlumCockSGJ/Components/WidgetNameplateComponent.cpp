#include "WidgetNameplateComponent.h"

#include "Subsystems/NpcSubsystem.h"

UWidgetNameplateComponent::UWidgetNameplateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWidgetNameplateComponent::BeginPlay()
{
	Super::BeginPlay();
	SetVisibility(false);
}

void UWidgetNameplateComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (IsVisible())
	{
		auto NpcSubsystem = GetWorld()->GetSubsystem<UNpcSubsystem>();
		if (NpcSubsystem)
		{
			FRotator NewRotation = NpcSubsystem->GetFacePlayerViewpointRotation(GetComponentLocation());
			SetWorldRotation(NewRotation);
		}
	}
}

