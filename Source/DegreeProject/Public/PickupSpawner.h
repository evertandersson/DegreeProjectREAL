// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawner.generated.h"

UCLASS()
class DEGREEPROJECT_API APickupSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupSpawner();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	class UBoxComponent* SpawnArea;

	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetRandomPointInVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
