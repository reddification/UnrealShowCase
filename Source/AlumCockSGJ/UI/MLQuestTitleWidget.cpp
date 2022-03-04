#include "UI/MLQuestTitleWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UMLQuestTitleWidget::SynchronizeProperties()
{
    Super::SynchronizeProperties();
    if (TitleText)
    {
        TitleText->SetText(DisplayText);
    }

    if (LineCross)
    {
        LineCross->SetRenderOpacity(IsComplited ? 1.0f : 0.0f);
    }

    if (CompletionMark)
    {
        CompletionMark->SetRenderOpacity(IsComplited ? 1.0f : 0.0f);
    }
}
