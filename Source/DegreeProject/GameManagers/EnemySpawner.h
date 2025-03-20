// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyRoundData.h"
#include "NPC.h"
#include "ObjectPooling.h"
#include "Containers/Map.h"
#include "EnemySpawner.generated.h"

UCLASS()
class DEGREEPROJECT_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemySpawner();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEnemyRoundData> AllRounds;

	UPROPERTY(VisibleAnywhere)
	int CurrentRound;

	UPROPERTY(VisibleAnywhere)
	int EnemiesKilled;

	UFUNCTION()
	void AddEnemyKilled();

	UPROPERTY(EditAnywhere)
	AObjectPooling* ObjectPool;


protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnNewRoundBegin();

	UFUNCTION()
	int GetTotalEnemiesThisRound();

};