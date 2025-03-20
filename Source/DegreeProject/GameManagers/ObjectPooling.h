// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PooledObject.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectPooling.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEGREEPROJECT_API AObjectPooling : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AObjectPooling();

	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	APooledObject* SpawnPooledObject(FVector Location, FRotator Rotation);

	UPROPERTY(EditAnywhere, Category = "Object Pool")
	TSubclassOf<class APooledObject> PooledObjectSubclass;

	UPROPERTY(EditAnywhere, Category = "Object Pool")
	int PoolSize = 20;

	UPROPERTY(EditAnywhere, Category = "Object Pool")
	float PooledObjectLifeSpan = 0.0f;

	UFUNCTION()
	void OnPooledObjectDespawn(APooledObject* PoolActor);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<APooledObject*> ObjectPool;
	UPROPERTY()
	TArray<int> SpawnedPoolIndexes;
};
