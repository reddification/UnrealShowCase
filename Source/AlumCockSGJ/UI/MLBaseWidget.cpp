#include "UI/MLBaseWidget.h"

void UMLBaseWidget::Show()
{
	SetVisibility(ESlateVisibility::Visible);
    if (ShowAnimation)
        PlayAnimation(ShowAnimation);
}

void UMLBaseWidget::Close()
{
	SetVisibility(ESlateVisibility::Hidden);
}
