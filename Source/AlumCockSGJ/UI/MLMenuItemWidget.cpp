// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MLMenuItemWidget.h"


/*class UCanvasPanelSlot;

void UMLMenuItemWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
    /*for ( int i = 0; i<10; i++)
    {
        // Instantiate the widget
        UMLQuestItemWidget* Entry = CreateWidget<UMLQuestItemWidget>( this, UMLQuestItemWidget::StaticClass(), FName("WBP_QuestItem") );
        // Set up its contents
        Entry->DisplayText = FText::FromString("3123123123132");
        Entry ->AddToViewport();

        // Add it to the list
        VBox->AddChildToVerticalBox( Entry );
    }*/

    // create image and set its position
  /*  UImage* image = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
    //image->SetBrushFromTexture(IndicatorTexture);
    image->SetColorAndOpacity(FLinearColor(0, 0, 0, 0));

    // add it to child of root
    UPanelWidget* root = Cast<UPanelWidget>(GetRootWidget());
    UPanelSlot* slot = root->AddChild(image);
    UCanvasPanelSlot* canvasSlot = Cast<UCanvasPanelSlot>(slot);

    // set position and stuff
    canvasSlot->SetAnchors(FAnchors(0.5f, 0.5f));
 //   canvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
 //   canvasSlot->SetOffsets(FMargin(0, 0, 100, 200));*/
/*
}*/

/*void UMLMenuItemWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UPanelWidget* root = Cast<UPanelWidget>(GetRootWidget());
    root->ClearChildren();
    
    for ( int i = 0; i<10; i++)
    {
        // Instantiate the widget
        UMLQuestItemWidget* Entry = CreateWidget<UMLQuestItemWidget>( this, UMLQuestItemWidget::StaticClass(), FName("WBP_QuestItem") );
        // Set up its contents
        Entry->DisplayText = FText::FromString("3123123123132");

        // Add it to the list
        root->AddChild( Entry );
    }
} */


