// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/GCLoadLastSaveWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"

DEFINE_LOG_CATEGORY_STATIC(LogGCLoadLastSaveWidget, All, All);

void UGCLoadLastSaveWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (LoadLastSaveButton)
    {
        LoadLastSaveButton->OnClicked.AddDynamic(this, &UGCLoadLastSaveWidget::OnLoadLastSave);
    }
}

void UGCLoadLastSaveWidget::OnLoadLastSave()
{
    if (!GetWorld())
        return;

    /* const auto GCGameInstance = GetWorld()->GetGameInstance<UGCGameInstance>();
       if (!GCGameInstance)
           return;

       if (GCGameInstance->GetMenuLevelName().IsNone())
       {
           UE_LOG(LogGCLoadLastSaveWidget, Error, TEXT("Menu level name is NONE"));
           return;
       }

       UGameplayStatics::OpenLevel(this, GCGameInstance->GetMenuLevelName());*/
}
