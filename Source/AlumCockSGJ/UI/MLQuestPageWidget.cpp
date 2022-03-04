// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MLQuestPageWidget.h"

#include "MLQuestItemWidget.h"
#include "Components/VerticalBox.h"

void UMLQuestPageWidget::SynchronizeProperties()
{
    Super::SynchronizeProperties();
    /*if (VBox)
    {
        VBox->ClearChildren();
        UMLQuestItemWidget* NewKid = CreateWidget<UMLQuestItemWidget>(this, UMLQuestItemWidget::StaticClass());
        NewKid->DisplayText = FText::FromString("SSSSSSS");
        NewKid->AddToViewport();
        VBox->AddChildToVerticalBox(
            NewKid
            );
    }*/
}
