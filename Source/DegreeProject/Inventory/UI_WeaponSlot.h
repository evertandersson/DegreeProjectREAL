#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_WeaponSlot.generated.h"

UCLASS()
class DEGREEPROJECT_API UUI_WeaponSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	// Function to update the weapon icon texture
	UFUNCTION(BlueprintCallable, Category = "Weapon Slot")
	void UpdateWeaponIcon(UTexture2D* NewWeaponTexture);

	UFUNCTION(BlueprintCallable, Category = "Weapon Slot")
	void UpdateHealingIcon(UTexture2D* NewHealingTexture);

};
