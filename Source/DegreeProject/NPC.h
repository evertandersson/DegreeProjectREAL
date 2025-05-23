// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "Damagable.h"
#include "UStandardAttributeSet.h"
#include "Enemy.h"
#include "Animation/AnimMontage.h"
#include "Delegates/DelegateCombinations.h"
#include "NPC.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMyEventDispatcher);

UCLASS()
class DEGREEPROJECT_API ANPC : public ACharacter, public IDamagable, public IEnemy
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

	void JumpToDestination_Implementation(FVector Destination);

	void DealDamageToPlayer_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Knockback(float KnockbackAmount);

	UFUNCTION(BlueprintCallable)
	void HandleKnockbackAnim(bool bIsJumpAttack);

	void PlayAnimation(UAnimInstance* AnimInstance, UAnimMontage* KnockbackAnim);

	void OnKnockbackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY(BlueprintAssignable, Category = "Events") // Only for dynamic multicast
	FMyEventDispatcher OnKnockbackEnd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<UAnimMontage*> KnockbackAnimations;

protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Attribute Set that stores and manages health and other attributes for replication.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	UStandardAttributeSet* AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", Replicated, meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent;

private:
	void ResetCollision();

	FTimerHandle ResetCollisionTimerHandle;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* KnockbackAnim;

	float ResetTimer;

	bool bIsDead = false;
};