// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Map.h"
#include "PoolData.generated.h"

USTRUCT(BlueprintType)
struct FPoolData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<TSubclassOf<AActor>, int32> ObjectCount;
};