// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h" 
#include "UStandardAttributeSet.h"
#include "PowerupsPickups.generated.h"

UCLASS()
class DEGREEPROJECT_API APowerupsPickups : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerupsPickups();

public:

	UPROPERTY(EditAnywhere)
	USceneComponent* PickUpRoot;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* PickUpBox;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PickUpMesh;

	FTimerHandle TimerHandle;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "HealthPickup")
	float value = 1.0f;

	UFUNCTION()
	void OnPlayerInteraction(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* otherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResults);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	FRotator RotationRate;

private:
	UPROPERTY()
	UStandardAttributeSet* AttributeSet;

	UFUNCTION()
	void ResetDefense();
};
