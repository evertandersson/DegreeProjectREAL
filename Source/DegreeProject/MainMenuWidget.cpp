// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"


#include "MainMenuWidget.h"  
#include "Blueprint/UserWidget.h"  
#include "Kismet/GameplayStatics.h"  
#include "Engine/World.h"  
#include "Kismet/KismetSystemLibrary.h"  
#include "Async/Async.h"  

void UMainMenuWidget::StartGame()  
{  
    UGameplayStatics::OpenLevel(this, FName("LoadingScreenLevel"));
}

void UMainMenuWidget::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}

void UMainMenuWidget::Options()
{

}
