// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SkillUpgrade.h"
#include "SkillUpgradeManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DEGREEPROJECT_API USkillUpgradeManager : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillUpgradeManager();

	// The pool of all possible upgrades
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FSkillUpgrade> AvailableUpgrades;

	// Function to get random upgrades
	UFUNCTION(BlueprintCallable)
	void GetRandomUpgrades(TArray<FSkillUpgrade>& OutPickedSkills);

	// Function to apply the chosen upgrade to the player
	UFUNCTION(BlueprintCallable)
	FSkillUpgrade& ApplyUpgrade(AActor* Player, int32 UpgradeIndex);
};