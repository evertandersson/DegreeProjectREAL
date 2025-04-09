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
