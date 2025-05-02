// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AFlyingEnemy::AFlyingEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->DefaultLandMovementMode = MOVE_Flying;
	GetCharacterMovement()->MovementMode = MOVE_Flying;
}

// Called when the game starts or when spawned
void AFlyingEnemy::BeginPlay()
{
	Super::BeginPlay();
	TargetActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	IsDead = false;
	
}

// Called every frame
void AFlyingEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TargetActor)
	{
		if (!IsDead)
		{
			float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
			
			
			if (DistanceToPlayer >= AcceptanceRadius)
			{
				MoveToTarget();
				
		    }
				
			FaceTarget(DeltaTime);
			
			if (DistanceToPlayer <= AcceptanceRadius)
			{
				MovementSpeed = 0.0f;
				AttackPlayer();
			}
		}
	}

	if (IsDead)
	{
		MovementSpeed = 0.0f;
	}
}

void AFlyingEnemy::FaceTarget(float DeltaTime)
{
	if (!TargetActor) return;

		FVector Direction = TargetActor->GetActorLocation() - GetActorLocation();
		FRotator LookAtRotaton = FRotationMatrix::MakeFromX(Direction).Rotator();

		FRotator TargetRotation(0.f, LookAtRotaton.Yaw, 0.f);

		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.f);
		SetActorRotation(NewRotation);
}

void AFlyingEnemy::MoveToTarget()
{
	if (!TargetActor) return;
	
	if (!Wall)
	{
		FVector Direction = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FVector Location = GetActorLocation() + Direction * MovementSpeed * GetWorld()->GetDeltaSeconds();
		SetActorLocation(Location);
		MovementSpeed = 400.0f;
	}
	
	FHitResult Hit;
	FVector Start =  GetActorLocation();
	FVector End = TargetActor->GetActorLocation();
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool hit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	if (hit)
	{
		Wall = true;
		FVector AvoidDirection = FVector::CrossProduct(Hit.Normal, FVector::UpVector).GetSafeNormal();
		FVector NewLocation = GetActorLocation() + AvoidDirection * MovementSpeed * GetWorld()->GetDeltaSeconds();
		SetActorLocation(NewLocation);
	}

	else
	{
		Wall = false;
	}

}



void AFlyingEnemy::AttackPlayer()
{
	IsAttacking = true;
	PlayAnimMontage(AttackAnimMontage);
	GetWorldTimerManager().SetTimer(AttackTime, this, &AFlyingEnemy::ResetAttack, 1.0f, false);

}

void AFlyingEnemy::ResetAttack()
{
	IsAttacking = false;
}




