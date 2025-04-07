// Fill out your copyright notice in the Description page of Project Settings.


#include "GrapplingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UGrapplingComponent::UGrapplingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bCanDisableGrapple = false;
	bCanGrapple = true;
	bIsRotatingTowardsGrapplePoint = false;
	bIsGrappling = false;
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

void UGrapplingComponent::RotateTowardsGrapplePoint(ADegreeProjectCharacter* Player, float Delta, float InterpSpeed)
{
	FVector PlayerLocation = Player->GetActorLocation();
	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, GrapplePoint);

	FRotator InterpTo = FMath::RInterpTo(Player->GetActorRotation(), LookAt, Delta, InterpSpeed);
	Player->SetActorRotation(InterpTo);
}

void UGrapplingComponent::ResetValues()
{
	bCanDisableGrapple = false;
	bIsRotatingTowardsGrapplePoint = false;
	bCanGrapple = true;
	bIsGrappling = false;
}

