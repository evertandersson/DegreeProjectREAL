// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ANPC::ANPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize the Attribute Set component for managing health and other attributes
	AttributeSet = CreateDefaultSubobject<UStandardAttributeSet>(TEXT("AttributeSet"));

	MaxHealth = 40.f;
	Health = MaxHealth;
}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANPC::TakeDamage_Implementation(UAbilitySystemComponent* AbilitySystem, bool bIsJumpAttack)
{

}

void ANPC::JumpToDestination_Implementation(FVector Destination)
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	
	FVector OutLaunchVelocity;
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(),
		OutLaunchVelocity,
		GetActorLocation(),
		FVector(Destination.X, Destination.Y, Destination.Z + 250.f));

	LaunchCharacter(OutLaunchVelocity, true, true);

	FVector RotToPlayer = Destination - GetActorLocation();
	RotToPlayer.Normalize();

	SetActorRotation(UKismetMathLibrary::MakeRotFromX(RotToPlayer));

	GetWorld()->GetTimerManager().SetTimer(ResetCollisionTimerHandle, this, &ANPC::ResetCollision, ResetTimer, false);
}

void ANPC::DealDamageToPlayer_Implementation()
{
}

void ANPC::ResetCollision()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}