// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponHolderComponent.h"
#include "Damagable.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UWeaponHolderComponent::UWeaponHolderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UWeaponHolderComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UWeaponHolderComponent::OnSwordHit(AActor* ThisActor, AActor* OtherActor, UAbilitySystemComponent* AbilitySystemComponent)
{
	bHitTarget = true;
	if (HitCameraShake)
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			PlayerController->ClientStartCameraShake(HitCameraShake);
		}
	}

	if (!EnemiesHit.Contains(OtherActor))
	{
		IDamagable::Execute_TakeDamage(OtherActor, AbilitySystemComponent);
		EnemiesHit.Add(OtherActor);
	}

	FVector ActorLoc = OtherActor->GetActorLocation();

	if (ImpactVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactVFX, ActorLoc, FRotator::ZeroRotator);
	}

	if (SoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundCue, ThisActor->GetActorLocation());
	}
}

void UWeaponHolderComponent::OnExplosionHit(AActor* OtherActor, UAbilitySystemComponent* AbilitySystemComponent)
{
	if (!EnemiesHit.Contains(OtherActor))
	{
		IDamagable::Execute_TakeDamage(OtherActor, AbilitySystemComponent);
		EnemiesHit.Add(OtherActor);
	}
}

