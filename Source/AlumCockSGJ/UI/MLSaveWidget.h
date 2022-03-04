// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MLSaveWidget.generated.h"

class UButton;


UCLASS()
class ALUMCOCKSGJ_API UMLSaveWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:
      UPROPERTY(meta = (BindWidget))
      UButton* SaveButton;
      virtual void NativeOnInitialized() override;

  private:
      UFUNCTION()
      void OnSave();
};