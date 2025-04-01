// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "UStandardAttributeSet.h"
#include "Pickup.h"
#include "DegreeProjectCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
AHealthPickup::AHealthPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PickUpRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PickUpRoot"));
	RootComponent = PickUpRoot;

	PickUpMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickUpMesh"));
	PickUpMesh->SetupAttachment(PickUpRoot);

	PickUpBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PickUpBox"));
	PickUpBox->SetupAttachment(RootComponent);
	PickUpBox->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	PickUpBox->OnComponentBeginOverlap.AddDynamic(this, &AHealthPickup::OnPlayerInteraction);

	
}

// Called when the game starts or when spawned
void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void AHealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHealthPickup::OnPlayerInteraction(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* otherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResults)
{
	//OnPickup(OverlappedComp, OtherActor, otherComp, OtherBodyIndex, bFromSweep, SweepResults);
	if (OtherActor->ActorHasTag("Player"))
	{
		UE_LOG(LogTemp, Warning, TEXT("hITT"));
		ADegreeProjectCharacter* Player = Cast<ADegreeProjectCharacter>(OtherActor);
		UStandardAttributeSet* AttributeSet = Player->GetAttributeSet();
		if (!AttributeSet)
		{
			return;
		}

		if (AttributeSet->CurrentHealth.GetCurrentValue() < AttributeSet->MaxHealth.GetCurrentValue())
		{
			UE_LOG(LogTemp, Warning, TEXT("HEAL"));
			AttributeSet->AddHealth(10.f);
			Destroy();
		}

		
	}
}

void AHealthPickup::OnPickup(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* otherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResults)
{
	
}