// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPC.h"
#include "Containers/Map.h"
#include "EnemyRoundData.generated.h"

USTRUCT(BlueprintType)
struct FEnemyRoundData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<TSubclassOf<ANPC>, int32> EnemyCount;
};