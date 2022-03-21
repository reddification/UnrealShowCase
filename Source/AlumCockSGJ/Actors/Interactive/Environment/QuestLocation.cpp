#include "QuestLocation.h"

#include "NavigationSystem.h"
#include "Characters/Controllers/BasePlayerController.h"
#include "Components/BoxComponent.h"
#include "Components/QuestGiverComponent.h"
#include "Data/Quests/QuestLocationDTR.h"
#include "Kismet/KismetMathLibrary.h"
#include "Subsystems/QuestSubsystem.h"
#include "Subsystems/WorldLocationsSubsystem.h"

AQuestLocation::AQuestLocation()
{
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Location Trigger"));
	CollisionComponent->SetupAttachment(RootComponent);

	QuestGiverComponent = CreateDefaultSubobject<UQuestGiverComponent>(TEXT("Quest giver"));
	AddOwnedComponent(QuestGiverComponent);
}

void AQuestLocation::BeginPlay()
{
	Super::BeginPlay();
	if (QuestLocationDTRH.DataTable && !QuestLocationDTRH.RowName.IsNone())
	{
		auto QuestLocationDTR = QuestLocationDTRH.GetRow<FQuestLocationDTR>("");
		if (QuestLocationDTR)
			bQuestLocation = QuestLocationDTR->bQuestLocation;
	}
	
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AQuestLocation::OnOverlapped);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AQuestLocation::OnExit);
	auto WLS = GetWorld()->GetSubsystem<UWorldLocationsSubsystem>();
	WLS->RegisterWorldLocation(this);
	GetWorld()->GetTimerManager().SetTimer(InitialOverlapTimer, this, &AQuestLocation::CheckOverlappingActors, 0.1f);
}

FVector AQuestLocation::GetRandomLocationInVolume(float FloorOffset) const
{
	// TODO shape sweep if you can actually place an actor here
	FVector ComponentLocation = CollisionComponent->GetComponentLocation();
	FVector ScaledBoxExtent = CollisionComponent->GetScaledBoxExtent();
	FVector NavigableSpawnLocation;
	bool bRandomNavigableLocationFound = UNavigationSystemV1::K2_GetRandomReachablePointInRadius(GetWorld(), ComponentLocation,
		NavigableSpawnLocation, FMath::Sqrt(ScaledBoxExtent.SizeSquared2D()));
	if (bRandomNavigableLocationFound)
		return NavigableSpawnLocation + FVector::UpVector * FloorOffset;
	
	FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox(ComponentLocation, ScaledBoxExtent * 0.9);
	RandomPoint.Z = ComponentLocation.Z - ScaledBoxExtent.Z + FloorOffset;
	return RandomPoint;
}

TArray<AActor*> AQuestLocation::GetOverlappedActorsInVolume(const TSubclassOf<AActor>& ActorTypeOfInterest) const
{
	TArray<AActor*> Result;
	CollisionComponent->GetOverlappingActors(Result, ActorTypeOfInterest);
	return Result;
}

void AQuestLocation::BeginDestroy()
{
	auto World = GetWorld();
	if (World)
	{
		auto WLS = World->GetSubsystem<UWorldLocationsSubsystem>();
		if (WLS)
			WLS->UnregisterWorldLocation(QuestLocationDTRH);
	}
	
	Super::BeginDestroy();
}

void AQuestLocation::OnOverlappedActor(AActor* OtherActor)
{
	if (bQuestLocation)
	{
		auto NpcCharacter = Cast<INpcCharacter>(OtherActor);
		if (NpcCharacter)
		{
			auto QuestSystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>();
			QuestSystem->OnLocationReached(QuestLocationDTRH, NpcCharacter->GetNpcDTRH());    
		}

		if (OtherActor->IsA<APlayerCharacter>())
			QuestGiverComponent->GiveQuests();
	}

	if (LocationGameplayTag.IsValid())
	{
		auto GameplayTagActor = Cast<IGameplayTagActor>(OtherActor);
		if (GameplayTagActor)
			GameplayTagActor->ChangeGameplayTags(LocationGameplayTag.GetSingleTagContainer());
	}
}

void AQuestLocation::OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnOverlappedActor(OtherActor);
}

void AQuestLocation::OnExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	if (LocationGameplayTag.IsValid())
	{
		auto GameplayTagActor = Cast<IGameplayTagActor>(OtherActor);
		if (GameplayTagActor)
			GameplayTagActor->ChangeGameplayTags(LocationGameplayTag.GetSingleTagContainer(), false);
	}
}


void AQuestLocation::OnLevelDeserialized_Implementation()
{
  
}

void AQuestLocation::CheckOverlappingActors()
{
	TArray<AActor*> InitiallyOverlappyingActors = GetOverlappedActorsInVolume(APawn::StaticClass());
	if (InitiallyOverlappyingActors.Num() > 0)
	{
		for (auto Actor : InitiallyOverlappyingActors)
			OnOverlappedActor(Actor);
	}
}
