// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UStandardAttributeSet.h"
#include "SkillUpgrade.generated.h"

USTRUCT(BlueprintType)
struct FSkillUpgrade
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Upgrade")
	TSubclassOf<UGameplayEffect> EffectType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Upgrade")
	bool bIsSkillUpgrade = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Upgrade")
	TSubclassOf<UGameplayAbility> SkillToUnlock;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Upgrade")
	float Level = 0.0f;
};