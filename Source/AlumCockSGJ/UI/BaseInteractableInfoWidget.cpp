#include "BaseInteractableInfoWidget.h"

#include "Components/TextBlock.h"

void UBaseInteractableInfoWidget::SetName(const FText& Text)
{
	if (NameTextblock)
		NameTextblock->SetText(Text);
}
