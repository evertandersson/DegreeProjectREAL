// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FlyingEnemiesAIController.generated.h"

/**
 * 
 */
UCLASS()
class DEGREEPROJECT_API AFlyingEnemiesAIController : public AAIController
{
	GENERATED_BODY()

public:

	AFlyingEnemiesAIController();
	
protected:

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

private:
	AActor* PlayerActor;
};
