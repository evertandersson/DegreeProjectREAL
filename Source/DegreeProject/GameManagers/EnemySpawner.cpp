// Fill out your copyright notice in the Description page of Project Settings.

#include "GameManagers/EnemySpawner.h"
#include "NavigationSystem.h"


// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CurrentRound = 0;
	EnemiesKilled = 0;
    Radius = 2000.f;
}


void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

    PoolSubsystem = GetWorld()->GetSubsystem<UPoolSubsystem>();

    GetGameManager();

    CreateWaveWidget();

    OnNewRoundBegin();
}

void AEnemySpawner::AddEnemyKilled(AActor* ActorToDespawn)
{
    // Increment the killed enemies counter
    EnemiesKilled++;

    // Despawn the actor
    PoolSubsystem->ReturnToPool(ActorToDespawn);

    UpdateEnemiesRemainingText(FMath::Max(0, TotalEnemiesThisRound - EnemiesKilled));

    const TMap<TSubclassOf<ANPC>, int32>& EnemyMap = AllRounds[CurrentRound].EnemyCount;

    // If we've killed all enemies for the round, start a new round
    if (EnemiesKilled >= TotalEnemiesThisRound)
    {
        RoundEnded();
    }
}

void AEnemySpawner::OnNewRoundBegin()
{
    CurrentRound++;
    EnemiesKilled = 0;
    TotalEnemiesThisRound = 0;

    // Ensure CurrentRound is within bounds
    if (!AllRounds.IsValidIndex(CurrentRound))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid CurrentRound index!"));
        return;
    }

    // Get the enemy count map for the current round
    const TMap<TSubclassOf<ANPC>, int32>& EnemyMap = AllRounds[CurrentRound].EnemyCount;

    PlannedEnemiesThisRound = 0;
    EnemyDataArray.Empty();
    for (const TPair<TSubclassOf<ANPC>, int32>& Pair : EnemyMap)
    {
        EnemyDataArray.Add(Pair);
        PlannedEnemiesThisRound += Pair.Value;
        UE_LOG(LogTemp, Warning, TEXT("Round %d: Added %s with count %d"),
            CurrentRound,
            *GetNameSafe(Pair.Key),
            Pair.Value);
    }

    // Initialize spawn counter and start the spawning process
    SpawnCounter = 0;
    SpawnedEnemiesPerType.Empty();

    // If this is the last round, set the timer for the spider
    if (CurrentRound == AllRounds.Num() - 1)
    {
        GetWorld()->GetTimerManager().SetTimer(CutsceneTimerHandle, this, &AEnemySpawner::SpawnEnemy, CutsceneTimer, false);
        return;
    }

    // Otherwise, spawn as normal
    GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AEnemySpawner::SpawnEnemy, SpawnInterval, true);
}

void AEnemySpawner::SpawnEnemy()
{
    if (SpawnCounter >= EnemyDataArray.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("All enemies spawned for round %d."), CurrentRound);
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle); // Stop the timer!
        UpdateEnemiesRemainingText(FMath::Max(0, TotalEnemiesThisRound - EnemiesKilled));
        return;
    }

    UpdateEnemiesRemainingText(FMath::Max(0, TotalEnemiesThisRound - EnemiesKilled));

    // Defensive check before accessing the array
    if (!EnemyDataArray.IsValidIndex(SpawnCounter))
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid SpawnCounter index %d! EnemyDataArray.Num() = %d"), SpawnCounter, EnemyDataArray.Num());
        return;
    }

    // Get the next enemy class and spawn count from the array
    const TPair<TSubclassOf<ANPC>, int32>& Pair = EnemyDataArray[SpawnCounter];
    TSubclassOf<ANPC> NPCClass = Pair.Key;

    // Check for null or invalid class
    if (!IsValid(NPCClass))
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid NPCClass in EnemyDataArray at index %d!"), SpawnCounter);
        // Skip to the next type instead of just returning (optional depending on design)
        SpawnCounter++;
        return;
    }

    SpawnCount = Pair.Value;

    // Check if we have already spawned all the enemies for this type
    int32 AlreadySpawnedCount = SpawnedEnemiesPerType.Contains(NPCClass) ? SpawnedEnemiesPerType[NPCClass] : 0;

    // If we haven't spawned all enemies of this type, spawn one more
    if (AlreadySpawnedCount < SpawnCount)
    {
        // Define the center of the circle
        FVector CircleCenter = GetActorLocation();
        float AngleStep = PlannedEnemiesThisRound > 0 ? 360.f / PlannedEnemiesThisRound : 0.f;
        float Angle = GlobalSpawnIndex * AngleStep;

        // Convert angle to radians for calculations
        float RadAngle = FMath::DegreesToRadians(Angle);
        float X = CircleCenter.X + Radius * FMath::Cos(RadAngle);
        float Y = CircleCenter.Y + Radius * FMath::Sin(RadAngle);
        //float Z = CircleCenter.Z;
        float Z = CircleCenter.Z + 2000;                    

        FHitResult Hit;
        FVector Start = FVector(X, Y, CircleCenter.Z + 1000.f);
        FVector End = FVector(X, Y, CircleCenter.Z - 1000.f);

        FCollisionQueryParams CollisionParams;
        CollisionParams.bTraceComplex = true;

        FVector SpawnLocation = FVector(X, Y, Z);
        FVector DesiredLocation = FVector(X, Y, Z);
        FRotator SpawnRotation = FRotator::ZeroRotator;
       
        FNavLocation ProjectedLocation;
        UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

        if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
        {
            DesiredLocation.Z = Hit.Location.Z + 500.f;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Line trace failed, using default Z"));
        }

        if (NavSys && NavSys->ProjectPointToNavigation(DesiredLocation, ProjectedLocation, FVector(500, 500, 500)))
        {
            SpawnLocation = ProjectedLocation.Location;
            DrawDebugSphere(GetWorld(), SpawnLocation, 50.f, 12, FColor::Green, false, 5.f);
        }
        else
        {
            SpawnLocation = DesiredLocation;
            UE_LOG(LogTemp, Error, TEXT("Spawn location is not on the NavMesh! Using original location."));
            DrawDebugSphere(GetWorld(), DesiredLocation, 50.f, 12, FColor::Red, false, 5.f);
        }

        if (PoolSubsystem)
        {
            AActor* SpawnedActor = nullptr;
            
            UE_LOG(LogTemp, Warning, TEXT("Spawning %s at location %s [Index: %d/%d]"),
                *GetNameSafe(NPCClass),
                *SpawnLocation.ToString(),
                AlreadySpawnedCount + 1,
                SpawnCount);

            // Attempt to spawn from pool with retries
            int32 RetryAttempts = 0;
            const int32 MaxRetries = 3;
            while (!SpawnedActor && RetryAttempts < MaxRetries)
            {
                PoolSubsystem->SpawnFromPool(NPCClass, SpawnLocation, SpawnRotation, SpawnedActor);
                RetryAttempts++;

                if (!SpawnedActor)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Retrying spawn for %s (Attempt %d/%d)"),
                        *GetNameSafe(NPCClass), RetryAttempts, MaxRetries);
                }
            }

            if (SpawnedActor)
            {
                UE_LOG(LogTemp, Warning, TEXT("Successfully spawned: %s"), *SpawnedActor->GetName());

                // Only increment if spawn succeeded
                SpawnedEnemiesPerType.Add(NPCClass, AlreadySpawnedCount + 1);
                GlobalSpawnIndex++;
                TotalEnemiesThisRound++;
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn actor of class: %s"), *GetNameSafe(NPCClass));
            }
        }
    }
    else
    {
        // Move to the next enemy type after we've spawned all of the current type
        SpawnCounter++;
    }
    
}