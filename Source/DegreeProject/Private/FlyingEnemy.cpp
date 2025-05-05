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
				
			

			FaceTarget(DeltaTime);
			
			

			FVector Start = GetActorLocation();
			FVector TargetLocation = TargetActor->GetActorLocation();
			FVector End = TargetLocation;
			FVector DirectionToTarget = (TargetLocation - Start).GetSafeNormal();

			FHitResult Hit;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);

			bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

			switch (CurrentState)
			{
			case EFlyAIState::MovingToPlayer:

				MovementSpeed = 400.0f;
				if (bHit && Hit.GetActor())
				{
					float ObstacleTopZ = Hit.ImpactPoint.Z + 300.f;
					float PlayerZ = TargetLocation.Z;

					if (ObstacleTopZ > Start.Z + 100.f) // obstacle is tall
					{
						TargetAscendZ = FMath::Max(PlayerZ + 200.f, ObstacleTopZ);
						CurrentState = EFlyAIState::AscendingOverObstacle;
						break;
					}
				}

				if ((TargetLocation - Start).Size() < 150.f)
				{
					CurrentState = EFlyAIState::AttackPlayer;
				}

				MoveInDirection(DirectionToTarget, DeltaTime);
				break;
			case EFlyAIState::AscendingOverObstacle:

				if(Start.Z<TargetAscendZ)
				{
					FVector Up = FVector::UpVector;
					MoveInDirection(Up, DeltaTime);
       			}

				else
				{
					CurrentState = EFlyAIState::DivingAtPlayer;
				}

				break;


			case EFlyAIState::DivingAtPlayer:
			{
				bool ObstacleStillThere = GetWorld()->LineTraceSingleByChannel(Hit, Start, TargetLocation, ECC_Visibility, Params);

				if (ObstacleStillThere && Hit.GetActor())
				{
					float ObstacleTopZ = Hit.ImpactPoint.Z + 300.f;
					float PlayerZ = TargetLocation.Z;

					if (ObstacleTopZ > Start.Z + 100.f)
					{
						TargetAscendZ = FMath::Max(PlayerZ + 200.f, ObstacleTopZ);
						CurrentState = EFlyAIState::AscendingOverObstacle;
						break;
					}
				}

				/*if ((TargetLocation - Start).Size() < 200.f)
				{
					CurrentState = EFlyAIState::MovingToPlayer;
				}*/

				if ((TargetLocation - Start).Size() < 150.f)
				{
					CurrentState = EFlyAIState::AttackPlayer;
				}

				MoveInDirection(DirectionToTarget, DeltaTime);
				break;

			}
			
			  case EFlyAIState::AttackPlayer:

				if (!IsAttacking)
				{
					IsAttacking = true;

					PlayAnimMontage(AttackAnimMontage);

					GetWorldTimerManager().SetTimer(AttackTime, this, &AFlyingEnemy::ResetAttack, 1.0f, false);
				}
				break;
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





void AFlyingEnemy::AttackPlayer()
{
	IsAttacking = true;
	MovementSpeed = 0.0f;
	PlayAnimMontage(AttackAnimMontage);
	GetWorldTimerManager().SetTimer(AttackTime, this, &AFlyingEnemy::ResetAttack, 1.0f, false);

}

void AFlyingEnemy::ResetAttack()
{
	IsAttacking = false;
	CurrentState = EFlyAIState::MovingToPlayer;
}

void AFlyingEnemy::MoveInDirection(const FVector& Direction, float DeltaTime)
{
	FVector NewLocation = GetActorLocation() + Direction * MovementSpeed * DeltaTime;
	SetActorLocation(NewLocation);
}




