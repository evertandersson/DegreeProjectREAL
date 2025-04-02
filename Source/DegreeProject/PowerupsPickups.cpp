// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerupsPickups.h"
#include "UStandardAttributeSet.h"
#include "DegreeProjectCharacter.h"
#include "Components/BoxComponent.h"


// Sets default values
APowerupsPickups::APowerupsPickups()
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
	PickUpBox->OnComponentBeginOverlap.AddDynamic(this, &APowerupsPickups::OnPlayerInteraction);


}

// Called when the game starts or when spawned
void APowerupsPickups::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void APowerupsPickups::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APowerupsPickups::OnPlayerInteraction(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* otherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResults)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		ADegreeProjectCharacter* Player = Cast<ADegreeProjectCharacter>(OtherActor);
		AttributeSet = Player->GetAttributeSet();

		AttributeSet->AddDefence(3);

		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APowerupsPickups::ResetDefense, 5.0f, false);
		UE_LOG(LogTemp, Warning, TEXT("ADDED DEFENSE"));
	}
}

void APowerupsPickups::ResetDefense()
{
	AttributeSet->Defence.SetCurrentValue(AttributeSet->Defence.GetCurrentValue() - 3);
	UE_LOG(LogTemp, Warning, TEXT("REMOVED DEFENSE"));
}


