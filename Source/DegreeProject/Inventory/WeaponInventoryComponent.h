// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/WeaponBase.h"
#include "WeaponInventoryComponent.generated.h"

class AWeaponBase;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEGREEPROJECT_API UWeaponInventoryComponent  : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UWeaponInventoryComponent ();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private: 
	UPROPERTY()
	TArray<AWeaponBase*> WeaponSlots;

	UPROPERTY()
	AWeaponBase* EquippedWeapon;

	UPROPERTY()
	int32 EquippedWeaponIndex;

public:	
	void AddWeapon(AWeaponBase* NewWeapon);
	void RemoveWeapon(AWeaponBase* Weapon);
	void EquipWeapon(int32 SlotIndex);
	void SwitchWeapon(int32 Direction);

	AWeaponBase* GetEquippedWeapon() const { return EquippedWeapon; }

};
