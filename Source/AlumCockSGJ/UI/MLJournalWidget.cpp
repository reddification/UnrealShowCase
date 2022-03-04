#include "UI/MLJournalWidget.h"

#include "MLQuestItemWidget.h"
#include "MLQuestTitleWidget.h"
#include "MLTaskItemWidget.h"
#include "Actors/Interactive/Environment/QuestLocation.h"
#include "Blueprint/WidgetTree.h"
#include "Gameframework/GameModeBase.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Game/MLGameInstance.h"
#include "Subsystems/QuestSubsystem.h"
#include "Subsystems/WorldLocationsSubsystem.h"
#include "Subsystems/WorldStateSubsystem.h"


class UMLQuestItemWidget;

void UMLJournalWidget::Show()
{
    auto QS = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>();
    auto JournalData = QS->GetJournalData();
    
    UpdateContent(JournalData);
    
    for (const auto& QuestItemWidgetDescriptor : QuestWidgets)
    {
        auto Widget = QuestWidgets[QuestItemWidgetDescriptor.Key];
        if (Widget->QuestItem == ActiveQuest)
        {
            ActiveQuestWidget = QuestWidgets[QuestItemWidgetDescriptor.Key];
            ActiveQuestWidget->SetActive(true);
            break;
        }
    }
    
    Super::Show();
}

void UMLJournalWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    if (ClearPauseButton)
    {
        ClearPauseButton->OnClicked.AddDynamic(this, &UMLJournalWidget::OnClearPause);
    }
}

void UMLJournalWidget::OnClearPause()
{
    if (!GetWorld() && !GetWorld()->GetAuthGameMode())
        return;

    if (ActiveQuestWidget)
        ActiveQuest = ActiveQuestWidget->QuestItem;
    
    GetWorld()->GetAuthGameMode()->ClearPause();
}

void UMLJournalWidget::AddVerticalSpacer() const
{
    USpacer* Spacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
    Spacer->SetSize(FVector2D((1.0, 10.0)));
    VBox->AddChildToVerticalBox(Spacer);
}

void UMLJournalWidget::UpdateContent(const FJournalModel& JournalModel)
{
    if (!GetWorld() && !GetWorld()->GetAuthGameMode())
        return;

    VBox->ClearChildren();
    ClearWidgets();

    if (QuestTitleClass)
    {
        UMLQuestTitleWidget* QuestTitleWidget = CreateWidget<UMLQuestTitleWidget>(this, QuestTitleClass);
        QuestTitleWidget->DisplayText = TextMainQuest;
        VBox->AddChildToVerticalBox(QuestTitleWidget);
    }

    AddVerticalSpacer();

    for (const FQuestItem QuestItem : JournalModel.Quests)
    {
        if (!QuestItem.IsAuxiliary && !QuestItem.IsCompleted)
        {
            if (QuestItemClass)
            {
                UMLQuestItemWidget* QuestItemWidget = CreateWidget<UMLQuestItemWidget>(this, QuestItemClass);
                QuestWidgets.Add(QuestItem.Id, QuestItemWidget);
                QuestItemWidget->QuestItem = QuestItem;
                QuestItemWidget->SetActive(false);
                QuestItemWidget->OnQuestItemActiveEvent.AddUObject(this, &UMLJournalWidget::OnChangeQuest);
                QuestItemWidget->SetPadding(FMargin(10.0, 0.0, 0.0, 0.0));

                VBox->AddChildToVerticalBox(QuestItemWidget);

                AddVerticalSpacer();

                for (FSubtaskItem SubtaskItem : QuestItem.Subtasks)
                {
                    UMLTaskItemWidget* TaskItemWidget = CreateWidget<UMLTaskItemWidget>(this, TaskItemClass);
                    TaskItemWidget->SubtaskItem = SubtaskItem;
                    TaskItemWidget->SetPadding(FMargin(115.0, 0.0, 0.0, 0.0));
                    VBox->AddChildToVerticalBox(TaskItemWidget);
                    AddVerticalSpacer();
                }
            }
        }
    }

    AddVerticalSpacer();

    if (QuestTitleClass)
    {
        UMLQuestTitleWidget* QuestTitleWidget = CreateWidget<UMLQuestTitleWidget>(this, QuestTitleClass);
        QuestTitleWidget->DisplayText = TextAuxiliaryQuest;
        VBox->AddChildToVerticalBox(QuestTitleWidget);
    }

    AddVerticalSpacer();

    for (const FQuestItem QuestItem : JournalModel.Quests)
    {
        if (QuestItem.IsAuxiliary && !QuestItem.IsCompleted)
        {
            if (QuestItemClass)
            {
                UMLQuestItemWidget* QuestItemWidget = CreateWidget<UMLQuestItemWidget>(this, QuestItemClass);

                QuestWidgets.Add(QuestItem.Id, QuestItemWidget);
                QuestItemWidget->QuestItem = QuestItem;
                QuestItemWidget->SetActive(false);
                QuestItemWidget->OnQuestItemActiveEvent.AddUObject(this, &UMLJournalWidget::OnChangeQuest);
                QuestItemWidget->SetPadding(FMargin(10.0, 0.0, 0.0, 0.0));

                VBox->AddChildToVerticalBox(QuestItemWidget);

                AddVerticalSpacer();

                for (FSubtaskItem SubtaskItem : QuestItem.Subtasks)
                {
                    UMLTaskItemWidget* TaskItemWidget = CreateWidget<UMLTaskItemWidget>(this, TaskItemClass);
                    TaskItemWidget->SubtaskItem = SubtaskItem;
                    TaskItemWidget->SetPadding(FMargin(115.0, 0.0, 0.0, 0.0));
                    VBox->AddChildToVerticalBox(TaskItemWidget);
                    AddVerticalSpacer();
                }
            }
        }
    }

    AddVerticalSpacer();

    if (QuestTitleClass)
    {
        UMLQuestTitleWidget* QuestTitleWidget = CreateWidget<UMLQuestTitleWidget>(this, QuestTitleClass);
        QuestTitleWidget->DisplayText = TextComplitedQuest;
        QuestTitleWidget->IsComplited = true;
        VBox->AddChildToVerticalBox(QuestTitleWidget);
    }

    AddVerticalSpacer();

    for (const FQuestItem QuestItem : JournalModel.Quests)
    {
        if (!QuestItem.IsAuxiliary && QuestItem.IsCompleted)
        {
            if (QuestItemClass)
            {
                UMLQuestItemWidget* QuestItemWidget = CreateWidget<UMLQuestItemWidget>(this, QuestItemClass);
                QuestItemWidget->QuestItem = QuestItem;
                QuestItemWidget->SetActive(false);
                QuestItemWidget->OnQuestItemActiveEvent.AddUObject(this, &UMLJournalWidget::OnChangeQuest);
                QuestItemWidget->SetPadding(FMargin(10.0, 0.0, 0.0, 0.0));

                VBox->AddChildToVerticalBox(QuestItemWidget);

                AddVerticalSpacer();
            }
        }
    }
}

void UMLJournalWidget::OnChangeQuest(const FName& Name)
{
    if (IsValid(ActiveQuestWidget))
    {
        const auto Widget = Cast<UMLQuestItemWidget>(ActiveQuestWidget);
        Widget->SetActive(false);
        DetailBox->SetRenderOpacity(0.0);
    }

    ActiveQuestWidget = QuestWidgets[Name];
    if (IsValid(ActiveQuestWidget))
    {
        const auto Widget = Cast<UMLQuestItemWidget>(ActiveQuestWidget);
        Widget->SetActive(true);

        QuestDetailTitle->SetText(Widget->QuestItem.Title);
        auto QS = GetGameInstance()->GetSubsystem<UQuestSubsystem>();

        auto NavigationGuidance = QS->GetNavigationGuidance(Widget->QuestItem.Id);
        if (NavigationGuidance.QuestLocation)
        {
            // TODO tell quest location to activate for compass system
            // NavigationGuidance.QuestLocation->ActivateForCompass();
            ActiveGuidance = NavigationGuidance;
            auto WSS = GetGameInstance()->GetSubsystem<UWorldStateSubsystem>();
            WorldStateChangeHandle = WSS->WorldStateChangedEvent.AddUObject(this, &UMLJournalWidget::OnWorldStateChanged);
        }
        
        TextDescription->SetText(Widget->QuestItem.Description);
        MapImage->SetBrushFromTexture(Widget->QuestItem.Map.Get(), false);
        MapImage->SetRenderOpacity(1.0);
        DetailBox->SetRenderOpacity(1.0);
    }
}

void UMLJournalWidget::OnWorldStateChanged(const FGameplayTagContainer& NewWorldState)
{
    if (ActiveGuidance.QuestLocation && ActiveGuidance.UntilWorldState.Matches(NewWorldState))
    {
        // TODO stop showing compass
        // ActiveGuidance.QuestLocation->StopShowingForCompass();
        ActiveGuidance.Clear();
    }
    
    auto WSS = GetGameInstance()->GetSubsystem<UWorldStateSubsystem>();
    WSS->WorldStateChangedEvent.Remove(WorldStateChangeHandle);
}

void UMLJournalWidget::ClearWidgets()
{
    for (auto It = QuestWidgets.CreateConstIterator(); It; ++It)
    {
        QuestWidgets.Remove(It.Key());
    }
}
