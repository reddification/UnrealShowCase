// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MLQuestItemWidget.h"

#include "FMODBlueprintStatics.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UMLQuestItemWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    if (MenuButton)
    {
        MenuButton->OnClicked.AddDynamic(this, &UMLQuestItemWidget::OnClick);
        MenuButton->OnHovered.AddDynamic(this, &UMLQuestItemWidget::OnHovered);
        MenuButton->OnUnhovered.AddDynamic(this, &UMLQuestItemWidget::OnUnhovered);
    }
}

void UMLQuestItemWidget::SynchronizeProperties()
{
    Super::SynchronizeProperties();
    if (TitleText)
    {
        TitleText->SetText(QuestItem.Title);
    }
    if (ImageComplited)
    {
        ImageComplited->SetRenderOpacity(QuestItem.IsCompleted ? 1.0f : 0.0f);
    }
}

void UMLQuestItemWidget::OnClick()
{
    if (!QuestItem.IsCompleted)
    {
        TitleText->SetColorAndOpacity(SelectedColor);
        if (ClickSFX)
        {
            UFMODBlueprintStatics::PlayEvent2D(GetWorld(), ClickSFX, true);
        }

        if (OnQuestItemActiveEvent.IsBound())
        {
            OnQuestItemActiveEvent.Broadcast(QuestItem.Id);
        }
    }
}

void UMLQuestItemWidget::SetActive(bool NewActive)
{
    IsActive = NewActive;
    ImageSelected->SetRenderOpacity(IsActive ? 1.0f : 0.0f);
}


void UMLQuestItemWidget::OnHovered()
{
    if (!QuestItem.IsCompleted)
    {
        TitleText->SetColorAndOpacity(HoveredColor);
    }
}

void UMLQuestItemWidget::OnUnhovered()
{
    if (!QuestItem.IsCompleted)
    {
        TitleText->SetColorAndOpacity(NormalColor);
    }
}
