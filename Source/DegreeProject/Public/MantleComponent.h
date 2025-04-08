// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MantleComponent.generated.h"


UENUM(BlueprintType)
enum class Enum_MantleType : uint8
{
	NoMantle UMETA(DisplayName = "NoMantle"),
	Mantle1M UMETA(DisplayName = "Mantle1M"),
	Mantle2M UMETA(DisplayName = "Mantle2M")
};

class ADegreeProjectCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEGREEPROJECT_API UMantleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMantleComponent();

	UFUNCTION(BlueprintCallable)
	float MantleTrace(ADegreeProjectCharacter* Player, float Length);

	UFUNCTION()
	bool IsObstacleInFront(ADegreeProjectCharacter* Player);

	UFUNCTION()
	float DetermineMantleHeight(ADegreeProjectCharacter* Player, const FHitResult& HitResult, float MaxAngle);

	UPROPERTY(BlueprintReadWrite)
	FVector MantleHeight;

	UPROPERTY(BlueprintReadWrite)
	Enum_MantleType MantleType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsClimbing;

};
