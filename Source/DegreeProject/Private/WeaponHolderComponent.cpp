// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponHolderComponent.h"
#include "Damagable.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

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

void UWeaponHolderComponent::ExplosionAttack(ACharacter* ThisActor)
{
	FString SocketName = "EndSword_VFX";

	if (!ThisActor->GetMesh()->DoesSocketExist(FName(SocketName)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket %s does not exist!"), *SocketName);
		return;
	}

	FVector SpawnLocation = ThisActor->GetMesh()->GetSocketLocation(FName(SocketName));

	// Create ExplosionHitbox if not created yet
	if (!ExplosionHitbox)
	{
		ExplosionHitbox = NewObject<USphereComponent>(this);
		ExplosionHitbox->InitSphereRadius(50.0f);  // Start small
		ExplosionHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // Enable only query collision (no physics)
		ExplosionHitbox->SetCollisionObjectType(ECC_WorldDynamic);
		ExplosionHitbox->SetCollisionResponseToAllChannels(ECR_Ignore);
		ExplosionHitbox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);  // Only overlap with enemies
		ExplosionHitbox->OnComponentBeginOverlap.AddDynamic(this, &UWeaponHolderComponent::OnExplosionHit);

		ExplosionHitbox->AttachToComponent(ThisActor->GetMesh(), FAttachmentTransformRules::KeepWorldTransform);
		ExplosionHitbox->SetWorldLocation(SpawnLocation);
		ExplosionHitbox->RegisterComponent();

		// Debug log for creation
		UE_LOG(LogTemp, Warning, TEXT("ExplosionHitbox Created at: %s"), *ExplosionHitbox->GetComponentLocation().ToString());

		// Extend the time before the hitbox is destroyed (increase this time for visibility)
		float HitboxLifeTime = 0.2f;  // Keep the hitbox alive for 0.2f second
		GetWorldTimerManager().SetTimer(DestroyHitboxTimerHandle, this, &UWeaponHolderComponent::DestroyExplosionHitbox, HitboxLifeTime, false);

		// Optional: Start expanding the hitbox to see it grow
		ExpandExplosionHitbox();
	}
}

void UWeaponHolderComponent::ExpandExplosionHitbox()
{
	if (ExplosionHitbox)
	{
		float CurrentRadius = ExplosionHitbox->GetUnscaledSphereRadius();

		// Gradually expand the hitbox
		if (CurrentRadius < MaxExplosionRadius)
		{
			// Expand the hitbox radius over time
			float NewRadius = FMath::Lerp(CurrentRadius, MaxExplosionRadius, 0.2f);
			ExplosionHitbox->SetSphereRadius(NewRadius);

			ExplosionHitbox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			// Drawing the expanding hitbox 
			DrawDebugSphere(GetWorld(), ExplosionHitbox->GetComponentLocation(), NewRadius, 12, FColor::Red, false, 0.1f, 0, 2.0f);

			// Continue expanding the hitbox
			GetWorldTimerManager().SetTimerForNextTick(this, &UWeaponHolderComponent::ExpandExplosionHitbox);
		}
		else
		{
			// Once max radius is reached, disable collision
			ExplosionHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			// Destroy the hitbox after a short delay
			GetWorldTimerManager().SetTimerForNextTick([this]() {
				if (ExplosionHitbox)
				{
					// Ensure the hitbox is destroyed and the pointer is cleared
					ExplosionHitbox->DestroyComponent();
					ExplosionHitbox = nullptr; // Clear the pointer after destruction
					UE_LOG(LogTemp, Warning, TEXT("ExplosionHitbox destroyed and pointer nullified"));
				}
				});
		}
	}
}