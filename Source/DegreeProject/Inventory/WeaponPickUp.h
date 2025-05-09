// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inventory/WeaponBase.h"
#include "WeaponPickUp.generated.h"

class USphereComponent;
class ADegreeProjectCharacter;

UCLASS()
class DEGREEPROJECT_API AWeaponPickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponPickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor)override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor)override;

	void PickupWeapon();

private:
	UPROPERTY(VisibleAnywhere, Category="Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AWeaponBase> WeaponClass;

	ADegreeProjectCharacter* OverlappingPlayer;
	UPROPERTY(EditAnywhere, Category = "Components")
	class UWidgetComponent* PickupWidget;
};
