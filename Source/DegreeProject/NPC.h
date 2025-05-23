// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "Damagable.h"
#include "UStandardAttributeSet.h"
#include "NPC.generated.h"

UCLASS()
class DEGREEPROJECT_API ANPC : public ACharacter, public IDamagable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void TakeDamage_Implementation(UAbilitySystemComponent* AbilitySystem, bool bIsJumpAttack) override;

	UFUNCTION(BlueprintImplementableEvent)
	void HandleDeath_Implementation();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float MaxHealth;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Attribute Set that stores and manages health and other attributes for replication.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	UStandardAttributeSet* AttributeSet;
};