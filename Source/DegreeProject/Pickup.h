// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h" 
#include "NiagaraComponent.h"
#include "UStandardAttributeSet.h"
#include "Pickup.generated.h"

UENUM(BlueprintType)
enum class PickupType : uint8
{
	NONE = 0		UMETA(DisplayName = "Invalid"),
	HEALTH			UMETA(DisplayName = "Health"),
	POWERUPS        UMETA(DisplayName = "Powerups"),

};

UCLASS()
class DEGREEPROJECT_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(Editanywhere)
	USceneComponent* PickUpRoot;

	UPROPERTY(Editanywhere)
	class UBoxComponent* PickUpBox;

	UPROPERTY(Editanywhere)
	UStaticMeshComponent* PickUpMesh;

	UPROPERTY(EditDefaultsOnly)
	PickupType pickupTypes;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	float value = 1.0f;

	UPROPERTY(Editanywhere)
	UNiagaraComponent* vfx;

	FTimerHandle TimerHandle;


	UFUNCTION()
	void OnPlayerInteraction(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* otherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResults);


private:

	UPROPERTY()
	UStandardAttributeSet* AttributeSets;

	UFUNCTION()
	void ResetDefense();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	FRotator RotationRate;
};
