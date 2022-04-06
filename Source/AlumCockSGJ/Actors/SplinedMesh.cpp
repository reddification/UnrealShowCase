#include "SplinedMesh.h"

#include "Engine/Classes/Components/InstancedStaticMeshComponent.h"
#include "Components/SplineComponent.h"

ASplinedMesh::ASplinedMesh()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SplineComponent->SetupAttachment(RootComponent);

	InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Instanced Static Mesh"));
	InstancedStaticMeshComponent->SetupAttachment(RootComponent);
}

void ASplinedMesh::BeginPlay()
{
	Super::BeginPlay();
}

void ASplinedMesh::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (!StaticMesh)
		return;

	InstancedStaticMeshComponent->ClearInstances();
	InstancedStaticMeshComponent->SetStaticMesh(StaticMesh);
	const auto& MeshBoundingBox = StaticMesh->GetBoundingBox();
	float ActualOffset = (MeshBoundingBox.Max - MeshBoundingBox.Min).X + Offset;
	if (ActualOffset <= 0.f)
		return;
	
	int InstancesCount = FMath::Floor(SplineComponent->GetSplineLength() / ActualOffset);
	FTransform NewInstanceTransform;
	for (auto i = 0; i < InstancesCount; i++)
	{
		FVector CurrentInstanceLocation = SplineComponent->GetLocationAtDistanceAlongSpline(i * ActualOffset, ESplineCoordinateSpace::Local);
		// FVector NextInstanceLocation = SplineComponent->GetLocationAtDistanceAlongSpline((i + 1) * ActualOffset, ESplineCoordinateSpace::Local);
		NewInstanceTransform.SetLocation(CurrentInstanceLocation);
		NewInstanceTransform.SetRotation(SplineComponent->GetRotationAtDistanceAlongSpline(i * ActualOffset, ESplineCoordinateSpace::Local).Quaternion());
		// NewInstanceTransform.SetRotation((NextInstanceLocation - CurrentInstanceLocation).ToOrientationQuat());
		InstancedStaticMeshComponent->AddInstance(NewInstanceTransform);
		// CurrentInstanceLocation = NextInstanceLocation;
	}
}
