// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPC.h"
#include "FlyingEnemy.generated.h"

UCLASS()
class DEGREEPROJECT_API AFlyingEnemy : public ANPC
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFlyingEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    void FaceTarget(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = "AI")
	float MovementSpeed = 400.f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AcceptanceRadius = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* TargetActor;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void MoveToTarget();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void MoveToNewLocation();

	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category = "AI")
	bool IsDead;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	bool Wall;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	bool IsAttacking;

	void AttackPlayer();
	void ResetAttack();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* AttackAnimMontage;

	FTimerHandle AttackTime;

	/*UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Health")
	void HandleDeath();
	
	virtual void HandleDeath_Implementation() override;*/
};
