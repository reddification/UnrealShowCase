#include "DialogueWidget.h"

void UDialogueWidget::ShowDialogueLine(const FQuestDialogueLine& Line, const FNpcDTR* Participant) const
{
	// TODO will it reset image if none was provided?
	SpeakerImageWidget->SetBrushFromTexture(Participant->Icon);
	SpeechTextWidget->SetText(Line.Text);
	SpeakerNameWidget->SetText(Participant->Name);
}
