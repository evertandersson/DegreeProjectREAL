#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/WeaponBase.h"
#include "WeaponInventoryComponent.generated.h"

class AWeaponBase;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEGREEPROJECT_API UWeaponInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UWeaponInventoryComponent();

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* SlotWidget;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TArray<AWeaponBase*> WeaponSlots;

	UPROPERTY(VisibleAnywhere)
	AWeaponBase* EquippedWeapon;

	UPROPERTY(VisibleAnywhere)
	int32 EquippedWeaponIndex;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SlotWidgetClass;

public:
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UUserWidget* WeaponSlotWidget;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetWeaponSlotWidget(UUserWidget* InWidget)
	{
		WeaponSlotWidget = InWidget;
	}

	void EquipWeapon(int32 SlotIndex);
	void AddWeapon(AWeaponBase* NewWeapon);
	void RemoveWeapon(AWeaponBase* Weapon);
	void SwitchWeapon(int32 Direction);

	AWeaponBase* GetEquippedWeapon() const { return EquippedWeapon; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void TryActivateCombo(int32 ComboIndex, AActor* InstigatorActor);
};
