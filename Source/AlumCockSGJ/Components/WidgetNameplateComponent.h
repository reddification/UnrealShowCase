#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "WidgetNameplateComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UWidgetNameplateComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UWidgetNameplateComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
