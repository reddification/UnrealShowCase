#include "BasePawnAnimInstance.h"

#include "Characters/BasePawn.h"
#include "GameFramework/PawnMovementComponent.h"

void UBasePawnAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<ABasePawn>(), TEXT("GCBasePawnAnimInstance Only GC Base Pawn supported"))
	Pawn = static_cast<ABasePawn*>(TryGetPawnOwner());
}

void UBasePawnAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	// UpdateAnimation can be called even before BeginPlay
	if (!Pawn.IsValid())
		return;
	
	InputForward = Pawn->GetInputForward();
	InputRight = Pawn->GetInputRight();
	bInAir = Pawn->MovementComponent->IsFalling();
}
