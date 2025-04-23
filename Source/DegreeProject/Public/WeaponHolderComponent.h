// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"
#include "WeaponHolderComponent.generated.h"

class ADegreeProjectCharacter;

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

	UPROPERTY()
	ADegreeProjectCharacter* PlayerCharacter;

public:	
	UFUNCTION()
	void OnSwordHit(AActor* ThisActor, AActor* OtherActor, UAbilitySystemComponent* AbilitySystemComponent);

	UFUNCTION()
	void OnExplosionOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AActor*> EnemiesHit;

	UFUNCTION(BlueprintCallable)
	void ExplosionAttack();

	UFUNCTION(BlueprintCallable)
	void DestroyExplosionHitbox();

	UFUNCTION()
	void ExpandExplosionHitbox();

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

	UPROPERTY()
	USphereComponent* ExplosionHitbox;

	FTimerHandle DestroyHitboxTimerHandle;
};
