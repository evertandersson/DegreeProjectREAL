// Fill out your copyright notice in the Description page of Project Settings.


#include "DegreeProject/UI/PauseMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

bool UPauseMenuWidget::Initialize()
{
    bool Success = Super::Initialize();
    if (!Success) return false;

    if (ResumeButton)
    {
        ResumeButton->OnPressed.AddDynamic(this, &UPauseMenuWidget::OnResumeButtonClicked);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ResumeButton not found!"));
    }

    if (MainMenuButton)
    {
        MainMenuButton->OnPressed.AddDynamic(this, &UPauseMenuWidget::OnMainMenuButtonClicked);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ResumeButton not found!"));
    }

    if (QuitButton)
    {
        QuitButton->OnPressed.AddDynamic(this, &UPauseMenuWidget::OnQuitButtonClicked);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("QuitButton not found!"));
    }

    return true;
}

void UPauseMenuWidget::OnResumeButtonClicked()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (!PlayerController) return;

    RemoveFromParent();

    PlayerController->SetPause(false);
    PlayerController->SetShowMouseCursor(false);
    PlayerController->SetInputMode(FInputModeGameOnly());
}

void UPauseMenuWidget::OnMainMenuButtonClicked()
{
    RemoveFromParent();

    UGameplayStatics::OpenLevel(this, FName("MainMenu"));
}

void UPauseMenuWidget::OnQuitButtonClicked()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}
