// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "UStandardAttributeSet.h"
#include "DegreeProjectCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
APickup::APickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PrimaryActorTick.bCanEverTick = true;

	PickUpRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PickUpRoot"));
	RootComponent = PickUpRoot;

	PickUpMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickUpMesh"));
	PickUpMesh->SetupAttachment(PickUpRoot);

	PickUpBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PickUpBox"));
	PickUpBox->SetupAttachment(RootComponent);
	PickUpBox->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	PickUpBox->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnPlayerInteraction);

	RotationRate = FRotator(0.0f, 90.0f, 0.0F);
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddActorLocalRotation(RotationRate * DeltaTime);

	float hight = 0.2 * FMath::Sin(GetWorld()->GetTimeSeconds());
	FVector pickupPosition = PickUpRoot->GetRelativeLocation();
	FVector Movement = FVector(pickupPosition.X, pickupPosition.Y, hight + pickupPosition.Z);
	PickUpRoot->SetRelativeLocation(Movement);

}

void APickup::OnPlayerInteraction(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* otherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResults)
{
	if (!OtherActor->ActorHasTag("Player")) return;
   
	if(pickupTypes == PickupType::HEALTH)
	{

		ADegreeProjectCharacter* Player = Cast<ADegreeProjectCharacter>(OtherActor);
		UStandardAttributeSet* AttributeSet = Player->GetAttributeSet();
		if (!AttributeSet)
		{
			return;
		}

		if (AttributeSet->CurrentHealth.GetCurrentValue() < AttributeSet->MaxHealth.GetCurrentValue())
		{

			AttributeSet->AddHealth(20.f);
			Destroy();
			UE_LOG(LogTemp, Warning, TEXT("HEAL"));
		}
	}

	if (pickupTypes == PickupType::POWERUPS)
	{
		ADegreeProjectCharacter* Player = Cast<ADegreeProjectCharacter>(OtherActor);
		AttributeSets = Player->GetAttributeSet();

		AttributeSets->AddDefence(3);

		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APickup::ResetDefense, 5.0f, false);
		UE_LOG(LogTemp, Warning, TEXT("ADDED DEFENSE"));
	}


}

void APickup::ResetDefense()
{
	AttributeSets->ResetDefance(3);
}

void APickup::OnPickup(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* otherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResults)
{

}


