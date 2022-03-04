#include "SubtitleWidget.h"

#include "Components/TextBlock.h"

void USubtitleWidget::Setup(const FText& SpeakerName, const FText& SpeechText, float Duration)
{
	SpeakerNameTextblock->SetText(SpeakerName);
	SpeechTextblock->SetText(SpeechText);
	GetWorld()->GetTimerManager().SetTimer(AutodestroyTimer, this, &USubtitleWidget::Autodestroy, Duration);
}

void USubtitleWidget::Autodestroy()
{
	RemoveFromParent();
}
