#include "SoakingComponent.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Data/DataAssets/Components/SoakingSettings.h"

USoakingComponent::USoakingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USoakingComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<ABaseHumanoidCharacter>(), TEXT("Soaking component only works with ABaseHumanoidCharacter"));
	checkf(IsValid(SoakingSettings), TEXT("Soaking Settings Data Asset must be set"));
	CharacterOwner = StaticCast<ABaseHumanoidCharacter*>(GetOwner());
	if (IsValid(SoakingSettings->SoakingCurve))
	{
		FOnTimelineFloatStatic SoakingTimelineCallback;
		SoakingTimelineCallback.BindUObject(this, &USoakingComponent::Soak);
		SoakingTimeline.AddInterpFloat(SoakingSettings->SoakingCurve, SoakingTimelineCallback);
		CardboardMaterial = CharacterOwner->GetMesh()->CreateDynamicMaterialInstance(0);
	}
}

void USoakingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (SoakingTimeline.IsPlaying())
		SoakingTimeline.TickTimeline(DeltaTime);
}

bool USoakingComponent::TryApplySoakness(float SoaknessGain)
{
	// TODO somehow increase soakness and start drying from some point at timeline if not in wet volume already
	return false;
}

void USoakingComponent::StartSoaking()
{
	PuddlesCount++;
	if (!SoakingTimeline.IsPlaying() || SoakingTimeline.IsReversing())
	{
		SoakingTimeline.SetPlayRate(SoakingSettings->SoakingRate);
		SoakingTimeline.Play();
	}
}

void USoakingComponent::StopSoaking()
{
	PuddlesCount--;
	if (PuddlesCount <= 0)
	{
		SoakingTimeline.SetPlayRate(SoakingSettings->DryingRate);
		SoakingTimeline.Reverse();
	}
}

void USoakingComponent::Soak(float NewSoakness)
{
	Soakness = NewSoakness;
	if (CardboardMaterial.IsValid())
	{
		CardboardMaterial->SetScalarParameterValue(SoakingSettings->SoakingParameterName, Soakness);			
	}
	
	GEngine->AddOnScreenDebugMessage(3, 0, FColor::Cyan, FString::Printf(TEXT("Soakness: %f"), Soakness));
}