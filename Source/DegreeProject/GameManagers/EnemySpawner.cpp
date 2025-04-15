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

    OnNewRoundBegin();
}

void AEnemySpawner::AddEnemyKilled(AActor* ActorToDespawn)
{
    // Despawn the actor
    PoolSubsystem->ReturnToPool(ActorToDespawn);

    // Increment the killed enemies counter
    EnemiesKilled++;

    // Calculate total enemies for the current round (sum of all values in the EnemyCount map)
    int32 TotalEnemiesThisRound = 0;
    const TMap<TSubclassOf<ANPC>, int32>& EnemyMap = AllRounds[CurrentRound].EnemyCount;
    for (const TPair<TSubclassOf<ANPC>, int32>& Pair : EnemyMap)
    {
        TotalEnemiesThisRound += Pair.Value;
    }

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
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AEnemySpawner::SpawnEnemy, 0.01f, true);
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
    SpawnCount = Pair.Value;

    // Check if we have already spawned all the enemies for this type
    int32 AlreadySpawnedCount = SpawnedEnemiesPerType.Contains(NPCClass) ? SpawnedEnemiesPerType[NPCClass] : 0;

    // If we haven't spawned all enemies of this type, spawn one more
    if (AlreadySpawnedCount < SpawnCount)
    {
        // Define the center of the circle
        FVector CircleCenter = GetActorLocation();
        float AngleStep = 360 / SpawnCount;
        float Angle = AlreadySpawnedCount * AngleStep;

        // Convert angle to radians for calculations
        float RadAngle = FMath::DegreesToRadians(Angle);
        float X = CircleCenter.X + Radius * FMath::Cos(RadAngle);
        float Y = CircleCenter.Y + Radius * FMath::Sin(RadAngle);
        //float Z = CircleCenter.Z;
        float Z = CircleCenter.Z + 350;                    

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

            // Call the function with a reference to SpawnedActor
            PoolSubsystem->SpawnFromPool(NPCClass, SpawnLocation, SpawnRotation, SpawnedActor);

            if (SpawnedActor)
            {
                // Optionally cast it to ANPC
                ANPC* SpawnedNPC = Cast<ANPC>(SpawnedActor);
                if (SpawnedNPC)
                {
                    // You can now use SpawnedNPC for additional setup if needed
                }

                // Increment the spawn count for this NPC type
               // SpawnedEnemiesPerType.Add(NPCClass, AlreadySpawnedCount + 1);
            }
        }
        // Increment the spawn count for this NPC type
        SpawnedEnemiesPerType.Add(NPCClass, AlreadySpawnedCount + 1);
        
    }
    else
    {
        // Move to the next enemy type after we've spawned all of the current type
        SpawnCounter++;
    }
    
}