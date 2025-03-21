// Fill out your copyright notice in the Description page of Project Settings.

#include "GameManagers/PoolSubsystem.h"
#include "GameManagers/Poolable.h"

AActor* UPoolSubsystem::SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation)
{
	AActor* PooledActor = nullptr;

	if (PoolClass.Get()->ImplementsInterface(UPoolable::StaticClass())) 
	{
		if (ObjectPool.IsEmpty())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			PooledActor = GetWorld()->SpawnActor<AActor>(PoolClass, Location, Rotation, SpawnParams);
		}
		else 
		{
			PooledActor = ObjectPool.Pop();
			PooledActor->SetActorLocationAndRotation(Location, Rotation);
		}

		IPoolable::Execute_OnSpawnFromPool(PooledActor);
	}

	return PooledActor;
}

void UPoolSubsystem::ReturnToPool(AActor* Poolable)
{
	if (Poolable->GetClass()->ImplementsInterface(UPoolable::StaticClass()))
	{
		IPoolable::Execute_OnReturnToPool(Poolable);
		ObjectPool.Add(Poolable);
	}
	else
	{
		Poolable->Destroy();
	}
}
