#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ComboAbilityBase.generated.h"

UCLASS()
class DEGREEPROJECT_API UComboAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UComboAbilityBase()
	{
		InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	}

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	int32 ComboIndex = 0;

	// The actual montage this ability will play
	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	UAnimMontage* MontageToPlay;

	// Set the correct montage based on combo index
	UFUNCTION(BlueprintCallable)
	void SetMontage(const TArray<UAnimMontage*>& Anims)
	{
		if (Anims.IsValidIndex(ComboIndex))
		{
			MontageToPlay = Anims[ComboIndex];
		}
	}
};
