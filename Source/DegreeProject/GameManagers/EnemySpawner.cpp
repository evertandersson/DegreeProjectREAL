// Fill out your copyright notice in the Description page of Project Settings.

#include "GameManagers/EnemySpawner.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CurrentRound = 0;
	EnemiesKilled = 0;
}


void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

    OnNewRoundBegin();
}

void AEnemySpawner::AddEnemyKilled()
{

}

void AEnemySpawner::OnNewRoundBegin()
{
    CurrentRound++;
    EnemiesKilled = 0;

    // Ensure CurrentRound is within bounds
    if (!AllRounds.IsValidIndex(CurrentRound))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid CurrentRound index!"));
        return;
    }

    // Get the enemy count map for the current round
    const TMap<TSubclassOf<ANPC>, int32>& EnemyMap = AllRounds[CurrentRound].EnemyCount;

    // Convert the TMap to an array of pairs for easier access by index
    EnemyDataArray.Empty();
    for (const TPair<TSubclassOf<ANPC>, int32>& Pair : EnemyMap)
    {
        EnemyDataArray.Add(Pair);
    }

    // Initialize spawn counter and start the spawning process
    SpawnCounter = 0;
    SpawnedEnemiesPerType.Empty(); // Reset this map to track spawned enemies per type

    // Start the timer to spawn enemies every 1 second
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AEnemySpawner::SpawnEnemy, 1.0f, true);
}

void AEnemySpawner::SpawnEnemy()
{
    // Check if we've spawned all enemies, then stop the timer
    if (SpawnCounter >= EnemyDataArray.Num())
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
        return;
    }

    // Get the next enemy class and spawn count from the array
    const TPair<TSubclassOf<ANPC>, int32>& Pair = EnemyDataArray[SpawnCounter];
    TSubclassOf<ANPC> NPCClass = Pair.Key;
    int32 SpawnCount = Pair.Value;

    // Check if we have already spawned all the enemies for this type
    int32 AlreadySpawnedCount = SpawnedEnemiesPerType.Contains(NPCClass) ? SpawnedEnemiesPerType[NPCClass] : 0;

    // If we haven't spawned all enemies of this type, spawn one more
    if (AlreadySpawnedCount < SpawnCount)
    {
        if (ObjectPool)
        {
            // Generate random location within a specific range
            float RandomX = FMath::RandRange(800.0f, 1000.0f); // Range for X coordinate
            float RandomY = FMath::RandRange(800.0f, 1000.0f); // Range for Y coordinate
            float RandomZ = 200.0f; // Set a fixed Z height (you can adjust this as needed)

            AActor* SpawnedActor = ObjectPool->SpawnFromPool(TEXT("Enemy"), FVector(RandomX, RandomY, RandomZ), FRotator::ZeroRotator);
            // Optionally, you can set the specific NPC class if necessary
            // SpawnedActor->SetClass(NPCClass);

            // Increment the spawn count for this NPC type
            SpawnedEnemiesPerType.Add(NPCClass, AlreadySpawnedCount + 1);
        }
    }
    else
    {
        // Move to the next enemy type after we've spawned all of the current type
        SpawnCounter++;
    }
}