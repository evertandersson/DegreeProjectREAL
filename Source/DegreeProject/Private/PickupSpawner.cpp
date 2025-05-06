// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupSpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APickupSpawner::APickupSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	RootComponent = SpawnArea;

}

FVector APickupSpawner::GetRandomPointInVolume()
{
	FVector Origin = SpawnArea->Bounds.Origin;
	FVector Extent = SpawnArea->Bounds.BoxExtent;
	return UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
}

void APickupSpawner::SpawnPickup()
{
	if (PickupClasses.Num() > 0)
	{
		int32 Index = FMath::RandRange(0, PickupClasses.Num() - 1);
		TSubclassOf<AActor> SelectedPickupClass = PickupClasses[Index];

		if (SelectedPickupClass)
		{
			FVector SpawnLocation = GetRandomPointInVolume();
			FRotator SpawnRotation = FRotator::ZeroRotator;

			GetWorld()->SpawnActor<AActor>(SelectedPickupClass, SpawnLocation, SpawnRotation);
		}
	}
}

// Called when the game starts or when spawned
void APickupSpawner::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &APickupSpawner::SpawnPickup, SpawnInterval, true);
}

// Called every frame
void APickupSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

}

