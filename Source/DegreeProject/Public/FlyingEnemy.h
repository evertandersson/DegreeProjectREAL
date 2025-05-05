// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPC.h"
#include "FlyingEnemy.generated.h"


UENUM(BlueprintType)
enum class EFlyAIState: uint8
{
	MovingToPlayer  UMETA(DisplayName = "Moving To Player"),
	AscendingOverObstacle UMETA(DisplayName = "Ascending Over Obstacle"),
	DivingAtPlayer  UMETA(DisplayName = "Diving At Player"),
	AttackPlayer    UMETA(DisplayName = "Attack Player"),
};

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

	EFlyAIState CurrentState = EFlyAIState::MovingToPlayer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    void FaceTarget(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = "AI")
	float MovementSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* TargetActor;

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

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* MeshComp;

	FTimerHandle AttackTime;

	void MoveInDirection(const FVector& Direction, float DeltaTime);

private:

	float TargetAscendZ = 0.f;
};
