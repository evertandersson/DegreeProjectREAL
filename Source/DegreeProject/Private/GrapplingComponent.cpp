// Fill out your copyright notice in the Description page of Project Settings.


#include "GrapplingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UGrapplingComponent::UGrapplingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bCanDisableGrapple = false;
}


// Called when the game starts
void UGrapplingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGrapplingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGrapplingComponent::LaunchTowardsGrapplePoint(ADegreeProjectCharacter* Player)
{
	UCharacterMovementComponent* CharacterMovement = Player->GetCharacterMovement();

	FVector PlayerLocation = Player->GetActorLocation();
	FVector Direction = GrapplePoint - PlayerLocation;
	Direction.Normalize();
	FVector ForcePower = Direction * 1000000.f;

	CharacterMovement->AddForce(ForcePower);

	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, GrapplePoint);

	Player->SetActorRotation(FRotator(Player->GetActorRotation().Pitch, LookAt.Yaw, LookAt.Roll));
}

bool UGrapplingComponent::IsBeyondGrapplePoint(ADegreeProjectCharacter* Player)
{
	bool bIsBeyondGrapplePoint = false;

	FVector Direction = GrapplePoint - Player->GetActorLocation();
	Direction.Normalize();

	FVector PlayerVelocity = Player->GetVelocity();
	PlayerVelocity.Normalize();

	bIsBeyondGrapplePoint = UKismetMathLibrary::Dot_VectorVector(Direction, PlayerVelocity) < 0.f
							&& bCanDisableGrapple;

	return bIsBeyondGrapplePoint;
}

