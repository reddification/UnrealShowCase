#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "ReadableWidget.generated.h"

class UTextBlock;
UCLASS()
class ALUMCOCKSGJ_API UReadableWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	bool TryOpenReadable(const FDataTableRowHandle& ReadableDTRH);
	void ChangePage(int NextPage);
	void PreviousPage();
	void NextPage();

protected:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* PagesTextblock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USoundCue* TurnPageSFX;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UOverlay* PaginationOverlay;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* CurrentPageCountWidget; 
	//
	// UPROPERTY(EditAnywhere, meta=(BindWidget))
	// UTextBlock* TotalPagesCountWidget;
	
private:
	int CurrentPage = 0;
	int PagesCount = 0;
	FDataTableRowHandle CurrentDTRH;
};
