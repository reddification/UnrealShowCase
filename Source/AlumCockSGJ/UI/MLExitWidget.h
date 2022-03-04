// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MLExitWidget.generated.h"


class UButton;

UCLASS()
class ALUMCOCKSGJ_API UMLExitWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
      UPROPERTY(meta = (BindWidget))
      UButton* ExitButton;
      virtual void NativeOnInitialized() override;

  private:
      UFUNCTION()
      void OnExit();
};