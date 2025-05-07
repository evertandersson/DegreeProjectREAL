// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupSpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APickupSpawner::APickupSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

FVector APickupSpawner::GetRandomPointInVolume()
{
	return UKismetMathLibrary::RandomPointInBoundingBox(SpawnBounds.GetCenter(), SpawnBounds.GetExtent());
}




void APickupSpawner::SpawnPickup()
{
	if (PickupClasses.Num() > 0)
	{
		int32 Index = FMath::RandRange(0, PickupClasses.Num() - 1);
		TSubclassOf<AActor> SelectedPickupClass = PickupClasses[Index];

		if (SelectedPickupClass)
		{
			FVector RandomPoint = GetRandomPointInVolume();
			FVector GroundLocation;

			if (FindGroundBelow(RandomPoint, GroundLocation))
			{
				FVector AdjustedLocation = GroundLocation + FVector(0, 0, 50.0f);
				FRotator SpawnRotation = FRotator::ZeroRotator;
				GetWorld()->SpawnActor<AActor>(SelectedPickupClass, AdjustedLocation, SpawnRotation);
			}
		}
	}
}

// Called when the game starts or when spawned
void APickupSpawner::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &APickupSpawner::SpawnPickup, SpawnInterval, true);
}

bool APickupSpawner::FindGroundBelow(FVector StartLocation, FVector& OutGroundLocation)
{
	FHitResult Hit;
	FVector EndLocation = StartLocation - FVector(0,0,5000);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Visibility, Params);
	if (bHit)
	{
		OutGroundLocation = Hit.Location;
	}

	return bHit;
}

// Called every frame
void APickupSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

}

