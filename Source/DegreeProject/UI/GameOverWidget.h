// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

class UButton;

UCLASS()
class DEGREEPROJECT_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;

public:
	UPROPERTY()
	UGameOverWidget* GameOverInstance;

	UFUNCTION()
	void OnRestartButtonClicked();

	UFUNCTION()
	void OnMainMenuButtonClicked();

	UFUNCTION()
	void OnQuitButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* RestartButton;

	UPROPERTY(meta = (BindWidget))
	UButton* MainMenuButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;
};
