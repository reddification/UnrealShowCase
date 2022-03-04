#include "CharacterInfoWidget.h"
#include "Components/ProgressBar.h"

void UCharacterInfoWidget::SetHealthPercent(float Percent)
{
	const auto HealthBarVisisbility =
		(Percent > PercentVisisbleThreshold || FMath::IsNearlyZero(Percent)) ? ESlateVisibility::Hidden : ESlateVisibility::Visible;

	HealthBar->SetVisibility(HealthBarVisisbility);

	const auto HealthBarColor = Percent > PercentColorThreshold ? GoodColor : BadColor;
	HealthBar->SetFillColorAndOpacity(HealthBarColor);
	HealthBar->SetPercent(Percent);
}
