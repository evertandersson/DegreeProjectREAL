// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "UStandardAttributeSet.h"
#include "DealDamage.generated.h"

/**
 * 
 */
UCLASS()
class DEGREEPROJECT_API UDealDamage : public UAnimNotify
{
	GENERATED_BODY()

public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Ainmation) override;
	
	UPROPERTY()
	UStandardAttributeSet* AttributeSets;

};
