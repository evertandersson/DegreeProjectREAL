// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameOverWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

bool UGameOverWidget::Initialize()
{
    bool Success = Super::Initialize();
    if (!Success) return false;

    if (RestartButton)
    {
        RestartButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnRestartButtonClicked);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("RestartButton not found!"));
    }

    if (MainMenuButton)
    {
        MainMenuButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnMainMenuButtonClicked);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ResumeButton not found!"));
    }

    if (QuitButton)
    {
        QuitButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnQuitButtonClicked);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("QuitButton not found!"));
    }

    return true;
}

void UGameOverWidget::OnRestartButtonClicked()
{
    UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (!PlayerController) return;

    RemoveFromParent();

    PlayerController->SetPause(false);
    PlayerController->SetShowMouseCursor(false);
    PlayerController->SetInputMode(FInputModeGameOnly());
}

void UGameOverWidget::OnMainMenuButtonClicked()
{
    RemoveFromParent();

    UGameplayStatics::OpenLevel(this, FName("MainMenu"));
}

void UGameOverWidget::OnQuitButtonClicked()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);

}
