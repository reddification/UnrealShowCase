#pragma once

#include "CoreMinimal.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "BasePawn.generated.h"

UCLASS()
class ALUMCOCKSGJ_API ABasePawn : public APawn
{
	GENERATED_BODY()

public:
	ABasePawn();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float val);
	void MoveRight(float val);
	
	UPROPERTY(EditAnywhere)
	class UPawnMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionComponent;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetInputForward() const {return InputForward; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetInputRight() const {return InputRight; }
	
	void Jump();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base pawn")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base pawn")
	class UCameraComponent* CameraComponent;

#if WITH_EDITORONLY_DATA 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base pawn")
	class UArrowComponent* ArrowComponent;
#endif

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base pawn")
	float CollisionSphereRadius = 50;

	
private:
	UFUNCTION()
	void OnBlendComplete();

	TWeakObjectPtr<AActor> CurrentViewActor = nullptr;
	TWeakObjectPtr<UCameraComponent> CurrentViewCameraComponent = nullptr;

	float InputForward = 0.0;
	float InputRight = 0.0;
};
