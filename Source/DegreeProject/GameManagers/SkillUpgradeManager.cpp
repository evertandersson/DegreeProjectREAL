// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManagers/SkillUpgradeManager.h"

USkillUpgradeManager::USkillUpgradeManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USkillUpgradeManager::GetRandomUpgrades(TArray<FSkillUpgrade>& OutPickedSkills)
{
    OutPickedSkills.Empty(); // Clear the array before adding new data
    const int32 TotalSkillsToShow = 3;

    if (AvailableUpgrades.Num() == 0)
    {
        return;
    }

    TArray<int32> AvailableIndices;
    for (int32 i = 0; i < AvailableUpgrades.Num(); i++)
    {
        AvailableIndices.Add(i);
    }

    for (int32 i = 0; i < TotalSkillsToShow && AvailableIndices.Num() > 0; i++)
    {
        int32 NumberToPick = FMath::RandRange(0, AvailableIndices.Num() - 1);
        OutPickedSkills.Add(AvailableUpgrades[AvailableIndices[NumberToPick]]);
        AvailableIndices.RemoveAt(NumberToPick);
    }
}

FSkillUpgrade& USkillUpgradeManager::ApplyUpgrade(AActor* Player, int32 UpgradeIndex)
{
    if (AvailableUpgrades.IsValidIndex(UpgradeIndex))
    {
        if (!AvailableUpgrades[UpgradeIndex].bIsSkillUpgrade) 
            AvailableUpgrades[UpgradeIndex].Level++;
        return AvailableUpgrades[UpgradeIndex];
    }
    return AvailableUpgrades[UpgradeIndex];
}

void USkillUpgradeManager::RemoveSkillFromAvailableUpgrades(int32 UpgradeIndex)
{
    if (AvailableUpgrades.IsValidIndex(UpgradeIndex))
    {
        AvailableUpgrades.RemoveAt(UpgradeIndex);
    }
}
