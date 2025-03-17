// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AMainMenuController::AMainMenuController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMainMenuController::BeginPlay()
{
	Super::BeginPlay();
	ShowMainMenu();
}

void AMainMenuController::ShowMainMenu()
{
	if (MainMenuWidgetClass)
	{
		UUserWidget* MainMenuWidget = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);

		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport();
			FInputModeUIOnly InputModeData;
			SetInputMode(InputModeData);
			bShowMouseCursor = true;
		}
	}
}

void AMainMenuController::StartGame()
{
	UGameplayStatics::OpenLevel(this, FName("ThirdPersonMap"));
}

void AMainMenuController::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}


