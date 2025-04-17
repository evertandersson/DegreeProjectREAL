// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingEnemiesAIController.h"
#include "Kismet/GameplayStatics.h"
#include "FlyingEnemy.h"

AFlyingEnemiesAIController::AFlyingEnemiesAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFlyingEnemiesAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void AFlyingEnemiesAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerActor)
	{
		AFlyingEnemy* Enemy = Cast<AFlyingEnemy>(GetPawn());
		if (Enemy)
		{
			Enemy->MoveToTarget(PlayerActor);
		}
	}

}
