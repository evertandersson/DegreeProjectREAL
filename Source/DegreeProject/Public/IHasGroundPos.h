// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IHasGroundPos.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIHasGroundPos : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEGREEPROJECT_API IIHasGroundPos : public IInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetGroundPos();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ToggleCanJump(bool CanJump);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void IsInStorm(bool bEnable);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LaunchCharacterInDirection(FVector Direction, bool bIsStorm);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetRotationBeforeRoll();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ConfirmGrappleHit();
};