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

    for (const TPair<TSubclassOf<ANPC>, int32>& Pair : EnemyMap)
    {
        TSubclassOf<ANPC> NPCClass = Pair.Key; // Get the enemy class
        int32 SpawnCount = Pair.Value;         // Get the number of enemies to spawn

        for (int32 i = 0; i < SpawnCount; i++)
        {
            GetWorld()->SpawnActor<ANPC>(NPCClass, GetActorLocation(), FRotator::ZeroRotator);
            // Spawn enemy from object pool (or use SpawnActor if no object pool is used)
            //if (ObjectPool)
            //{
            //    ObjectPool->SpawnPooledObject(NPCClass); // Assuming your object pool supports this
            //}
            //else
            //{
            //    GetWorld()->SpawnActor<ANPC>(NPCClass, GetActorLocation(), FRotator::ZeroRotator);
            //}
        }
    }
}

int AEnemySpawner::GetTotalEnemiesThisRound()
{
	return 0;
}
