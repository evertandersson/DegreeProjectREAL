#include "UI_WeaponSlot.h"
#include "Components/Image.h"

void UUI_WeaponSlot::UpdateWeaponIcon(UTexture2D* NewWeaponTexture)
{
	if (NewWeaponTexture)
	{
		UImage* WeaponImage = Cast<UImage>(GetWidgetFromName(TEXT("WeaponIcon")));  // Assuming your Image is named "WeaponIcon"
		if (WeaponImage)
		{
			WeaponImage->SetBrushFromTexture(NewWeaponTexture);
		}
	}
}

void UUI_WeaponSlot::UpdateHealingIcon(UTexture2D* NewWeaponTexture)
{
	if (NewWeaponTexture)
	{
		UImage* HealingIcon = Cast<UImage>(GetWidgetFromName(TEXT("HealingIcon")));  // Assuming your Image is named "WeaponIcon"
		if (HealingIcon)
		{
			HealingIcon->SetBrushFromTexture(NewWeaponTexture);
		}
	}
}