// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbilitySystemComponent.h"
#include "Damagable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UDamagable : public UInterface
{
	GENERATED_BODY()
};

class DEGREEPROJECT_API IDamagable
{
	GENERATED_BODY()

public:
	// Declare function with BlueprintNativeEvent to allow C++ and Blueprint overrides
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Health")
	void TakeDamage(UAbilitySystemComponent* AbilitySystem);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Health")
	void HandleDeath();

};
