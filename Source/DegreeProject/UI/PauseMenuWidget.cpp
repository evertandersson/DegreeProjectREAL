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
        ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeButtonClicked);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ResumeButton not found!"));
    }

    if (MainMenuButton)
    {
        MainMenuButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnMainMenuButtonClicked);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ResumeButton not found!"));
    }
    if (QuitButton)
    {
        MainMenuButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnQuitButtonClicked);
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
    UGameplayStatics::OpenLevel(this, FName("MainMenu"));
}

void UPauseMenuWidget::OnQuitButtonClicked()
{
    UE_LOG(LogTemp, Error, TEXT("ITS GETTING CALLED"))

    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
    
}
