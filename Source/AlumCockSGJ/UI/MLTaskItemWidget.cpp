#include "UI/MLTaskItemWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void UMLTaskItemWidget::SynchronizeProperties()
{
    Super::SynchronizeProperties();
    if (TitleText)
    {
        TitleText->SetText(SubtaskItem.Title);
    }

    if (ImageComplited)
    {
        ImageComplited->SetRenderOpacity(SubtaskItem.IsCompleted ? 1.0f : 0.0f);
    }
}