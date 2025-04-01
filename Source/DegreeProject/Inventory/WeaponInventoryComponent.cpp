// Fill out your copyright notice in the Description page of Project Settings.
#include "Kismet/KismetSystemLibrary.h"
#include "Inventory/WeaponBase.h"
#include "Inventory/WeaponInventoryComponent.h"

// Sets default values
UWeaponInventoryComponent ::UWeaponInventoryComponent ()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void UWeaponInventoryComponent ::BeginPlay()
{
	Super::BeginPlay();
}

void UWeaponInventoryComponent ::AddWeapon(AWeaponBase* NewWeapon)
{
	if (NewWeapon && !WeaponSlots.Contains(NewWeapon))
	{
		WeaponSlots.Add(NewWeapon);
	}
}

void UWeaponInventoryComponent ::RemoveWeapon(AWeaponBase* Weapon)
{
	if (WeaponSlots.Contains(Weapon))
	{
		if (EquippedWeapon == Weapon)
		{
			EquippedWeapon = nullptr;
		}
		WeaponSlots.Remove(Weapon);
	}
}

void UWeaponInventoryComponent ::EquipWeapon(int32 SlotIndex)
{
	if (WeaponSlots.IsValidIndex(SlotIndex))
	{
		EquippedWeapon = WeaponSlots[SlotIndex];
		EquippedWeaponIndex = SlotIndex;
	}
}

void UWeaponInventoryComponent::SwitchWeapon(int32 Direction)
{
	if (WeaponSlots.Num() == 0) 
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponInvenoryComponent: No weapons in inventory!"))
		return; //in case there is no weapon to switch to
	}

	int32 NewIndex = EquippedWeaponIndex + Direction;

	//Warp around the index if out of bounds
	if (NewIndex >= WeaponSlots.Num())
	{
		NewIndex = 0; //go to first Weapon
	}
	else if (NewIndex < 0)
	{
		NewIndex = WeaponSlots.Num() - 1; //To the last weapon
	}

	UE_LOG(LogTemp, Warning, TEXT("Switching weapon: OldIndex = %d, NewIndex = %d"), EquippedWeaponIndex, NewIndex);

	// Debug message on screen (Optional)
	FString DebugMessage = FString::Printf(TEXT("Switched to Weapon Index: %d"), NewIndex);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, DebugMessage);
}

