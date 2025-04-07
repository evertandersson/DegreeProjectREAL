// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
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
	virtual void TakeDamage(float DamageAmount) = 0;
};
