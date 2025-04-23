// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrapplingComponent.generated.h"

class ADegreeProjectCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEGREEPROJECT_API UGrapplingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrapplingComponent();

	UFUNCTION(BlueprintCallable)
	void LaunchTowardsGrapplePoint(ADegreeProjectCharacter* Player);

	UFUNCTION(BlueprintCallable)
	bool IsBeyondGrapplePoint(ADegreeProjectCharacter* Player);

	UFUNCTION(BlueprintCallable)
	void RotateTowardsGrapplePoint(ADegreeProjectCharacter* Player, float Delta, float InterpSpeed);

	UFUNCTION(BlueprintCallable)
	void ResetValues();

	UFUNCTION(BlueprintCallable)
	bool ShouldSetNewEndPoint();

	UPROPERTY(BlueprintReadWrite)
	FVector GrapplePoint;

	UPROPERTY(BlueprintReadWrite)
	bool bCanDisableGrapple;

	UPROPERTY(BlueprintReadWrite)
	bool bCanGrapple;

	UPROPERTY(BlueprintReadWrite)
	bool bIsRotatingTowardsGrapplePoint;

	UPROPERTY(BlueprintReadWrite)
	bool bIsGrappling;

	UPROPERTY(BlueprintReadWrite)
	bool bIsAiming;

	UPROPERTY(BlueprintReadWrite)
	bool bHasAvailableGrapplingPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bIsLaunchingGrapple;

};
