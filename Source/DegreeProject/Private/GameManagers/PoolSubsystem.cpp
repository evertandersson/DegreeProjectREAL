// Fill out your copyright notice in the Description page of Project Settings.

#include "GameManagers/PoolSubsystem.h"
#include "GameManagers/Poolable.h"

void UPoolSubsystem::SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation, AActor*& SpawnedActor)
{
	SpawnedActor = SpawnFromPool<AActor>(PoolClass, Location, Rotation);
}

void UPoolSubsystem::ReturnToPool(AActor* Poolable)
{
	const UClass* PoolableClass = Poolable->GetClass();
	if (PoolableClass->ImplementsInterface(UPoolable::StaticClass()))
	{
		IPoolable::Execute_OnReturnToPool(Poolable);
		FPoolArray* ObjectPool = ObjectPools.Find(PoolableClass);
		ObjectPool->Add(Poolable);
	}
	else
	{
		Poolable->Destroy();
	}
}
