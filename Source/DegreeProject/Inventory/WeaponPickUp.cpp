// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/WeaponPickup.h"
#include "Components/SphereComponent.h"
#include "WeaponInventoryComponent.h"
#include "DegreeProjectCharacter.h"

// Sets default values
AWeaponPickUp::AWeaponPickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(50.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = CollisionComponent;
}

// Called when the game starts or when spawned
void AWeaponPickUp::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponPickUp::NotifyActorBeginOverlap(AActor* OtherActor)
{
	ADegreeProjectCharacter* Player = Cast<ADegreeProjectCharacter>(OtherActor);
	if (Player)
	{
		OverlappingPlayer = Player;
		UE_LOG(LogTemp, Warning, TEXT("Player can pick up weapon! Press E"));
	}
}

void AWeaponPickUp::NotifyActorEndOverlap(AActor* OtherActor)
{
	ADegreeProjectCharacter* Player = Cast<ADegreeProjectCharacter>(OtherActor);
	if (Player && Player == OverlappingPlayer)
	{
		OverlappingPlayer = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Player left pickup range."));
	}
}

void AWeaponPickUp::PickupWeapon()
{
	if (OverlappingPlayer && WeaponClass)
	{
		UWeaponInventoryComponent* Inventory = OverlappingPlayer->FindComponentByClass<UWeaponInventoryComponent>();
		if (Inventory)
		{
			AWeaponBase* NewWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass);
			if (NewWeapon)
			{
				Inventory->AddWeapon(NewWeapon);
				Destroy(); // Remove the pickup after collection
			}
		}
	}
}

