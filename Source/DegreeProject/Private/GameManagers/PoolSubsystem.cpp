#include "GameManagers/PoolSubsystem.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "UObject/UObjectGlobals.h"

void UPoolSubsystem::SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation, AActor*& SpawnedActor)
{
	SpawnedActor = SpawnFromPool<AActor>(PoolClass, Location, Rotation);
}

void UPoolSubsystem::ReturnToPool(AActor* Poolable)
{
	if (!Poolable)
	{
		UE_LOG(LogTemp, Warning, TEXT("ReturnToPool: Attempted to return a null actor."));
		return;
	}

	UClass* PoolableClass = Poolable->GetClass();  // <== changed from const UClass* to UClass*

	if (PoolableClass->ImplementsInterface(UPoolable::StaticClass()))
	{
		IPoolable::Execute_OnReturnToPool(Poolable);
		FPoolArray& ObjectPool = ObjectPools.FindOrAdd(PoolableClass);  // now works!
		ObjectPool.Add(Poolable);
		UE_LOG(LogTemp, Log, TEXT("Returned actor %s to pool."), *Poolable->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ReturnToPool: Actor %s does not implement IPoolable, destroying."), *Poolable->GetName());
		Poolable->Destroy();
	}
}

void UPoolSubsystem::PrewarmPool(TSubclassOf<AActor> PoolClass, int32 Count)
{
	if (!IsValid(PoolClass) || !PoolClass->ImplementsInterface(UPoolable::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("PrewarmPool: Invalid or non-poolable class."));
		return;
	}

	for (int32 i = 0; i < Count; ++i)
	{
		AActor* TempActor = nullptr;
		SpawnFromPool(PoolClass, FVector::ZeroVector, FRotator::ZeroRotator, TempActor);
		if (TempActor)
		{
			ReturnToPool(TempActor);
		}
	}
}

template<typename T>
T* UPoolSubsystem::SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation)
{
	T* PooledActor = nullptr;

	if (!IsValid(PoolClass))
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnFromPool: Invalid class passed in."));
		return nullptr;
	}

	if (!PoolClass->ImplementsInterface(UPoolable::StaticClass()))
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnFromPool: Class %s does not implement IPoolable."), *GetNameSafe(PoolClass));
		return nullptr;
	}

	FPoolArray& ObjectPool = ObjectPools.FindOrAdd(PoolClass);

	if (ObjectPool.IsEmpty())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		PooledActor = GetWorld()->SpawnActor<T>(PoolClass, Location, Rotation, SpawnParams);

		if (PooledActor)
		{
			UE_LOG(LogTemp, Log, TEXT("SpawnFromPool: Spawned NEW actor %s."), *PooledActor->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("SpawnFromPool: Failed to spawn actor of class %s."), *GetNameSafe(PoolClass));
			return nullptr;
		}
	}
	else
	{
		AActor* RawActor = ObjectPool.Pop();
		PooledActor = Cast<T>(RawActor);

		if (!PooledActor)
		{
			UE_LOG(LogTemp, Error, TEXT("SpawnFromPool: Failed to cast pooled actor of class %s."), *GetNameSafe(PoolClass));
			return nullptr;
		}

		PooledActor->SetActorLocationAndRotation(Location, Rotation);
		UE_LOG(LogTemp, Log, TEXT("SpawnFromPool: Reused actor from pool: %s."), *PooledActor->GetName());
	}

	IPoolable::Execute_OnSpawnFromPool(PooledActor);
	return PooledActor;
}
