// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Map.h"
#include "PoolData.generated.h"

USTRUCT(BlueprintType)
struct FPoolData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
    FName Tag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
    TSubclassOf<AActor> Prefab;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
    int32 Size = 10;
};