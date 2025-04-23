// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"
#include "DegreeProjectCharacter.h"
#include "WeaponHolderComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEGREEPROJECT_API UWeaponHolderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponHolderComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	void OnSwordHit(AActor* ThisActor, AActor* OtherActor, UAbilitySystemComponent* AbilitySystemComponent);

	UFUNCTION()
	void OnExplosionHit(AActor* OtherActor, UAbilitySystemComponent* AbilitySystemComponent);

	UFUNCTION(BlueprintCallable)
	void ExplosionAttack(ACharacter* ThisActor);

	UFUNCTION()
	void ExpandExplosionHitbox();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AActor*> EnemiesHit;

	UPROPERTY(EditAnywhere, Category = "Explosion Attack")
	float MaxExplosionRadius = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Explosion Attack")
	float ExpansionTime = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Explosion Attack")
	float ExplosionForce = 1500.0f;

protected:
	UPROPERTY(EditAnywhere, Category = "Combat|VFX")
	UParticleSystem* ImpactVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|SFX")
	USoundCue* SoundCue;

	UPROPERTY(BlueprintReadWrite)
	bool bHitTarget = false;

	UPROPERTY(EditAnywhere, Category = "Camera Shake")
	TSubclassOf<UCameraShakeBase> HitCameraShake;

private:
	UPROPERTY()
	USphereComponent* ExplosionHitbox;

	FTimerHandle DestroyHitboxTimerHandle;

	FVector InitialLocation;  // Stores the location before attack starts
	FTimerHandle AttackTimer;
};
