// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sound/SoundCue.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AActor*> EnemiesHit;

protected:
	UPROPERTY(EditAnywhere, Category = "Combat|VFX")
	UParticleSystem* ImpactVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|SFX")
	USoundCue* SoundCue;

	UPROPERTY(BlueprintReadWrite)
	bool bHitTarget = false;

	UPROPERTY(EditAnywhere, Category = "Camera Shake")
	TSubclassOf<UCameraShakeBase> HitCameraShake;
};
