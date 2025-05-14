// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameManagers/Poolable.h"
#include "PoolSubsystem.generated.h"

USTRUCT()
struct FPoolArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AActor*> ObjectPool;

	bool IsEmpty() const
	{
		return ObjectPool.IsEmpty();
	}

	AActor* Pop()
	{
		return ObjectPool.Pop();
	}

	void Add(AActor* ActorToAdd)
	{
		ObjectPool.Add(ActorToAdd);
	}
};

UCLASS()
class DEGREEPROJECT_API UPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Pool Subsystem", meta = (DeterminesOutputType = "PoolClass", DynamicOutputParam = "SpawnedActor"))
	void SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation, AActor*& SpawnedActor);

	UFUNCTION(BlueprintCallable, Category = "Pool Subsystem")
	void ReturnToPool(AActor* Poolable);

	UFUNCTION(BlueprintCallable, Category = "Pool Subsystem")
	void PrewarmPool(TSubclassOf<AActor> PoolClass, int32 Count);

protected:
	UPROPERTY()
	TMap<UClass*, FPoolArray> ObjectPools;

private:
	template<typename T>
	T* SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation);
};

