#include "ObjectPooling.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"


AObjectPooling* AObjectPooling::Instance = nullptr;

AObjectPooling::AObjectPooling()
{
    PrimaryActorTick.bCanEverTick = false;
}

AObjectPooling::~AObjectPooling()
{
    for (auto& Pair : PoolDictionary)
    {
        delete Pair.Value;  // Free memory
    }
    PoolDictionary.Empty();
}

AObjectPooling* AObjectPooling::GetInstance(UWorld* World)
{
    if (!Instance)
    {
        for (TActorIterator<AObjectPooling> It(World); It; ++It)
        {
            Instance = *It;
            break;
        }
    }
    return Instance;
}

void AObjectPooling::BeginPlay()
{
    Super::BeginPlay();
    InitializePools();

    if (GEngine)
    {
        FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &AObjectPooling::OnLevelLoaded);
    }
}

void AObjectPooling::InitializePools()
{
    // Clear previous pools
    PoolDictionary.Empty();

    for (const FPoolData& Pool : Pools)
    {
        if (!Pool.Prefab)
        {
            UE_LOG(LogTemp, Error, TEXT("Pool '%s' has a null prefab!"), *Pool.Tag.ToString());
            continue;
        }

        // Create a queue dynamically (Heap allocation)
        TQueue<AActor*, EQueueMode::Spsc>* ObjectPool = new TQueue<AActor*, EQueueMode::Spsc>();

        for (int32 i = 0; i < Pool.Size; i++)
        {
            AActor* Object = GetWorld()->SpawnActor<AActor>(Pool.Prefab, FVector(10000, 10000, 10000), FRotator::ZeroRotator);
            if (Object)
            {
                Object->SetActorHiddenInGame(true);
                Object->SetActorEnableCollision(false);
                Object->SetActorTickEnabled(false);
                ObjectPool->Enqueue(Object);
            }
        }

        // Store a pointer to the queue
        PoolDictionary.Add(Pool.Tag, ObjectPool);
    }
}



AActor* AObjectPooling::SpawnFromPool(FName Tag, FVector Location, FRotator Rotation)
{
    if (!PoolDictionary.Contains(Tag))
    {
        UE_LOG(LogTemp, Warning, TEXT("Pool with tag %s doesn't exist."), *Tag.ToString());
        return nullptr;
    }

    AActor* ObjectToSpawn = nullptr;
    TQueue<AActor*, EQueueMode::Spsc>* ObjectPool = PoolDictionary[Tag];

    // If the pool is empty, handle reusing an active object
    if (ObjectPool->IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Pool %s is empty! Reusing the oldest active object."), *Tag.ToString());

        // If there are active actors, despawn the first one and reuse it
        if (ActiveActors.Num() > 0)
        {
            // Get the oldest active actor (FIFO - first-in, first-out)
            AActor* OldestActor = ActiveActors[0];

            // Despawn this object
            DespawnObject(OldestActor);

            // Remove the oldest actor from the active list
            ActiveActors.RemoveAt(0);

            // Reuse this actor for the new spawn
            ObjectToSpawn = OldestActor;
            ObjectToSpawn->SetActorLocation(Location);
            ObjectToSpawn->SetActorRotation(Rotation);
            ObjectToSpawn->SetActorHiddenInGame(false);
            ObjectToSpawn->SetActorEnableCollision(true);
            ObjectToSpawn->SetActorTickEnabled(true);
        }
        else
        {
            // If there are no active actors, we can't reuse one from the pool
            UE_LOG(LogTemp, Warning, TEXT("No active actors to reuse. Cannot spawn."));
            return nullptr;
        }
    }
    else
    {
        // If there are objects in the pool, just dequeue one
        if (ObjectPool->Dequeue(ObjectToSpawn) && ObjectToSpawn)
        {
            // Initialize the object for use
            ObjectToSpawn->SetActorLocation(Location);
            ObjectToSpawn->SetActorRotation(Rotation);
            ObjectToSpawn->SetActorHiddenInGame(false);
            ObjectToSpawn->SetActorEnableCollision(true);
            ObjectToSpawn->SetActorTickEnabled(true);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to dequeue object from pool %s."), *Tag.ToString());
            return nullptr;
        }
    }

    // Before adding the object to ActiveActors, ensure it's not already there
    if (!ActiveActors.Contains(ObjectToSpawn))
    {
        // After spawning or reusing the object, re-enqueue it to be reused later
        ObjectPool->Enqueue(ObjectToSpawn);

        // Track this actor as active
        ActiveActors.Add(ObjectToSpawn);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor already exists in the active actors array."));
    }

    return ObjectToSpawn;
}


void AObjectPooling::InitializeActor()
{

}



void AObjectPooling::DespawnObject(AActor* ObjectToDespawn)
{
    if (ObjectToDespawn)
    {
        ObjectToDespawn->SetActorHiddenInGame(true);
        ObjectToDespawn->SetActorEnableCollision(false);
        ObjectToDespawn->SetActorTickEnabled(false);
        ObjectToDespawn->SetActorLocation(FVector(10000, 10000, 10000));
    }
}

void AObjectPooling::OnLevelLoaded(UWorld* LoadedWorld, const UWorld::InitializationValues InitValues)
{
    InitializePools();
}

void AObjectPooling::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (GEngine)
    {
        FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
    }

    Super::EndPlay(EndPlayReason);
}
