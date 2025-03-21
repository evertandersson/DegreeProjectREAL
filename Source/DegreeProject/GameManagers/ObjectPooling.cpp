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

    if (!PoolDictionary[Tag]->Dequeue(ObjectToSpawn) || !ObjectToSpawn)
    {
        UE_LOG(LogTemp, Error, TEXT("Spawned object from pool %s is null!"), *Tag.ToString());
        return nullptr;
    }

    ObjectToSpawn->SetActorLocation(Location);
    ObjectToSpawn->SetActorRotation(Rotation);
    ObjectToSpawn->SetActorHiddenInGame(false);
    ObjectToSpawn->SetActorEnableCollision(true);
    ObjectToSpawn->SetActorTickEnabled(true);

    PoolDictionary[Tag]->Enqueue(ObjectToSpawn);

    return ObjectToSpawn;
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
