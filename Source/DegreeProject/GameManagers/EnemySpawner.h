#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyRoundData.h"
#include "NPC.h"
#include "Containers/Map.h"
#include "TimerManager.h" 
#include "GameManagers/PoolSubsystem.h"
#include "EnemySpawner.generated.h"

UCLASS()
class DEGREEPROJECT_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemySpawner();

	// Array holding the enemy rounds data
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEnemyRoundData> AllRounds;

	// Current round number
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int CurrentRound;

	// Number of enemies killed in the current round
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int EnemiesKilled;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 SpawnCount;

	// Counter to keep track of the number of different enemy types being spawned
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int SpawnCounter;

	// Function to call when an enemy is killed
	UFUNCTION(BlueprintCallable)
	void AddEnemyKilled(AActor* ActorToDespawn);

	UFUNCTION(BlueprintImplementableEvent)
	void RoundEnded();

	// Called when a new round begins
	UFUNCTION(BlueprintCallable)
	void OnNewRoundBegin();

	UFUNCTION(BlueprintImplementableEvent)
	void GetGameManager();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Radius;

	UFUNCTION(BlueprintImplementableEvent)
	void CreateWaveWidget();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateEnemiesRemainingText(int EnemiesRemaining);

	int PlannedEnemiesThisRound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int TotalEnemiesThisRound;

protected:
	virtual void BeginPlay() override;

private:
	// Timer handle for spawning enemies
	FTimerHandle SpawnTimerHandle;

	float SpawnInterval = 0.01f;

	UPROPERTY()
	int GlobalSpawnIndex = 0;

	TArray<TPair<TSubclassOf<ANPC>, int32>> EnemyDataArray;

	// Map to track how many enemies have been spawned for each NPC class type
	TMap<TSubclassOf<ANPC>, int32> SpawnedEnemiesPerType;

	// Function to spawn one enemy at a time
	UFUNCTION()
	void SpawnEnemy();

	UPROPERTY()	
	UPoolSubsystem* PoolSubsystem;
};