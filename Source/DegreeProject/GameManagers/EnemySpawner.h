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
	UPROPERTY(VisibleAnywhere)
	int CurrentRound;

	// Number of enemies killed in the current round
	UPROPERTY(VisibleAnywhere)
	int EnemiesKilled;

	// Function to call when an enemy is killed
	UFUNCTION(BlueprintCallable)
	void AddEnemyKilled(AActor* ActorToDespawn);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when a new round begins
	UFUNCTION()
	void OnNewRoundBegin();

private:
	// Timer handle for spawning enemies
	FTimerHandle SpawnTimerHandle;

	// Counter to keep track of the number of different enemy types being spawned
	int SpawnCounter;

	TArray<TPair<TSubclassOf<ANPC>, int32>> EnemyDataArray;

	// Map to track how many enemies have been spawned for each NPC class type
	TMap<TSubclassOf<ANPC>, int32> SpawnedEnemiesPerType;

	// Function to spawn one enemy at a time
	UFUNCTION()
	void SpawnEnemy();

	UPROPERTY()
	UPoolSubsystem* PoolSubsystem;
};