// Fill out your copyright notice in the Description page of Project Settings.
#include "Inventory/WeaponInventoryComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Inventory/UI_WeaponSlot.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/WeaponBase.h"

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

	if (SlotWidgetClass)
	{
		SlotWidget = CreateWidget<UUserWidget>(GetWorld(), SlotWidgetClass);
		if (SlotWidget)
		{
			UUI_WeaponSlot* WeaponSlot = Cast<UUI_WeaponSlot>(SlotWidget);
			if (WeaponSlot)
			{
				WeaponSlotWidget = WeaponSlot;
				WeaponSlotWidget->AddToViewport(1);
			}
		}
	}
}

void UWeaponInventoryComponent::AddWeapon(AWeaponBase* NewWeapon)
{
	if (NewWeapon)
	{
		WeaponSlots.Add(NewWeapon);

		if (WeaponSlots.Num() == 1)
		{
			EquipWeapon(0);
		}

		// Show all inventory slots in debug log
		FString InventoryList = "Current Inventory: ";
		for (int32 i = 0; i < WeaponSlots.Num(); i++)
		{
			InventoryList += FString::Printf(TEXT("[%d] %s "), i, *WeaponSlots[i]->GetName());
		}

		UE_LOG(LogTemp, Warning, TEXT("Weapon Added! %s"), *InventoryList);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, InventoryList);
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

void UWeaponInventoryComponent::EquipWeapon(int32 SlotIndex)
{
	if (WeaponSlots.IsValidIndex(SlotIndex))
	{
		EquippedWeapon = WeaponSlots[SlotIndex];
		EquippedWeaponIndex = SlotIndex;

		UE_LOG(LogTemp, Warning, TEXT("Equipped Weapon at Index: %d"), EquippedWeaponIndex);

		// Debug on-screen message
		FString DebugMessage = FString::Printf(TEXT("Equipped Weapon at Index: %d"), EquippedWeaponIndex);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, DebugMessage);

		for (AWeaponBase* Weapon : WeaponSlots)
		{
			if (Weapon && Weapon != EquippedWeapon)
			{
				Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				Weapon->SetActorHiddenInGame(true);
			}
		}

		if (AActor* OwnerActor = GetOwner())
		{
			USkeletalMeshComponent* Mesh = OwnerActor->FindComponentByClass<USkeletalMeshComponent>();
			if (Mesh)
			{
				EquippedWeapon->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("FirstHandSocket"));
				EquippedWeapon->SetActorHiddenInGame(false);
			}
		}
		if (WeaponSlotWidget)
		{
			UUI_WeaponSlot* WeaponSlot = Cast<UUI_WeaponSlot>(WeaponSlotWidget);
			if (WeaponSlot)
			{
				UTexture2D* WeaponIcon = EquippedWeapon->GetWeaponIcon();
				WeaponSlot->UpdateWeaponIcon(WeaponIcon);
			}
		}
	}

	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Equip Index: %d"), SlotIndex);
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

	EquipWeapon(NewIndex);

	UE_LOG(LogTemp, Warning, TEXT("Switching weapon: OldIndex = %d, NewIndex = %d"), EquippedWeaponIndex, NewIndex);

	// Debug message on screen (Optional)
	FString DebugMessage = FString::Printf(TEXT("Switched to Weapon Index: %d"), NewIndex);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, DebugMessage);
}

void UWeaponInventoryComponent::TryActivateCombo(int32 ComboIndex, AActor* InstigatorActor)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->ActivateCombo(ComboIndex, InstigatorActor);
	}
}

