#include "ReadableWidget.h"

#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Data/Entities/ReadableDTR.h"

bool UReadableWidget::TryOpenReadable(const FDataTableRowHandle& ReadableDTRH)
{
	if (ReadableDTRH.IsNull())
		return false;
	
	FReadableDTR* ReadableDTR = ReadableDTRH.DataTable->FindRow<FReadableDTR>(ReadableDTRH.RowName, "");
	if (!ReadableDTR || ReadableDTR->Pages.Num() <= 0)
		return false;

	CurrentDTRH = ReadableDTRH;
	CurrentPage = 0;
	PagesTextblock->SetText(ReadableDTR->Pages[0]);
	PagesCount = ReadableDTR->Pages.Num();
	if (PagesCount > 1)
	{
		CurrentPageCountWidget->SetText(FText::AsNumber(1));
		// TotalPagesCountWidget->SetText(FText::AsNumber(PagesCount));
		PaginationOverlay->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		PaginationOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
	
	return true;
}

void UReadableWidget::PreviousPage()
{
	int NextPage = CurrentPage > 0 ? CurrentPage - 1 : CurrentPage;
	ChangePage(NextPage);	
}

void UReadableWidget::NextPage()
{
	int NextPage = CurrentPage < PagesCount - 1 ? CurrentPage + 1 : CurrentPage;
	ChangePage(NextPage);
}

void UReadableWidget::ChangePage(int NextPage)
{
	if (NextPage == CurrentPage)
		return;

	CurrentPage = NextPage;
	auto ReadableDTR = CurrentDTRH.DataTable->FindRow<FReadableDTR>(CurrentDTRH.RowName, ""); 
	PagesTextblock->SetText(ReadableDTR->Pages[NextPage]);
	if (PagesCount > 1)
		CurrentPageCountWidget->SetText(FText::AsNumber(CurrentPage + 1));
	
	if (IsValid(TurnPageSFX))
		UFMODBlueprintStatics::PlayEvent2D(GetWorld(), TurnPageSFX, true);
}