#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SplinedMesh.generated.h"

UCLASS()
class ALUMCOCKSGJ_API ASplinedMesh : public AActor
{
	GENERATED_BODY()

public:
	ASplinedMesh();
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USplineComponent* SplineComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 0.f, ClampMin = 0.f))
	float Offset = 0.f;
};
