// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "KnightAnimationClass.generated.h"

/**
 * 
 */
UCLASS()
class DEGREEPROJECT_API UKnightAnimationClass : public UAnimInstance
{
	GENERATED_BODY()

public:
    // This variable will be controlled from C++
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
    bool bIsAttacking = false;
};
