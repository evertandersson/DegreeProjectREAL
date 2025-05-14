// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawner.generated.h"

UCLASS()
class DEGREEPROJECT_API APickupSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupSpawner();

	UPROPERTY(EditAnywhere, Category = "Spawning")
	FBox SpawnBounds = FBox(FVector(-16000, -14000, 0), FVector(13000, 10000, 4000));

	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetRandomPointInVolume();

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TArray<TSubclassOf<AActor>> PickupClasses;

	UFUNCTION()
	void DecreaseSpawnedPickups();

private:
	FTimerHandle SpawnTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float SpawnInterval = 5.0f;

	void SpawnPickup();

	UPROPERTY(EditAnywhere, Category = "Spawning")
	int MaxLimit = 10;

	UPROPERTY()
	int PickupsSpawned = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool FindGroundBelow(FVector StartLocation, FVector& OutGroundLocation);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
