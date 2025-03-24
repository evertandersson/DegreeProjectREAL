// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UStandardAttributeSet.h"
#include "SkillUpgrade.generated.h"

UENUM(BlueprintType)
enum class EUpgradeType : uint8
{
	HEALTH_INCREASE UMETA(DisplayName = "Health Increase"),
	MANA_INCREASE UMETA(DisplayName = "Mana Increase"),
	STAMINA_INCREASE UMETA(DisplayName = "Stamina Increase"),
	CRIT_CHANCE_INCREASE UMETA(DisplayName = "Crit Chance Increase"),
	CRIT_DAMAGE_INCREASE UMETA(DisplayName = "Crit Damage Increase"),
	DAMAGE_INCREASE UMETA(DisplayName = "Damage Increase"),
	DEFENSE_INCREASE UMETA(DisplayName = "Defense Increase"),
	ATTACK_SPEED_INCREASE UMETA(DisplayName = "Attack Speed Increase"),
	SKILL UMETA(DisplayName = "Skill")
};

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