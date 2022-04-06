#pragma once

#include "CoreMinimal.h"
#include "UITypes.h"
#include "Blueprint/UserWidget.h"
#include "MLBaseWidget.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UMLBaseWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    virtual void Show();

    UFUNCTION(BlueprintCallable)
    virtual void Close();

    mutable FChangeMouseCursorEvent ChangeMouseCursorEvent;
    
protected:
    UPROPERTY(meta=(BindWidgetAnim), Transient)
    UWidgetAnimation* ShowAnimation;
};
