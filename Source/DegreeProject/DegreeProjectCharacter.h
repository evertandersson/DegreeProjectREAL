// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include <GameplayEffectTypes.h>

#include "Blueprint/UserWidget.h"
#include "Inventory/WeaponInventoryComponent.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"

#include "UStandardAttributeSet.h"
#include "DegreeProjectCharacter.generated.h"

class UWeaponInventoryComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UPauseMenuWidget;
class UGameOverWidget;
class UMyAbilitySystemComponent;
class UAbilitySystemComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UENUM(BlueprintType)
enum class EGASAbilityInputID : uint8
{
	None UMETA(DisplayName = "None"),
	Confirm UMETA(DisplayName = "Confirm"),
	Cancel UMETA(DisplayName = "Cancel")
};

UCLASS(config=Game)
class ADegreeProjectCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Roll Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RollAction;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PauseAction;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwitchWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PickUpWeapons;
public:
	ADegreeProjectCharacter();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Implement the interface method to return the Ability System
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Blueprint event to handle health changes and update the UI
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void TakeDamage(float DamageAmount);

	void HandleDeath();
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxMana = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxStamina = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxStat = 99; //Change later if needed



	//Dash mechanic
	void Dash();
	void StopDash();
	void ResetDashCoolDown();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void GiveDefualtAbilities();

	UStandardAttributeSet* GetAttributeSet();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

protected:
	// Ability System Component that manages attributes and effects.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", Replicated, meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent;

	// Attribute Set that stores and manages health and other attributes for replication.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes", Replicated, meta = (AllowPrivateAccess = "true"))
	UStandardAttributeSet* AttributeSet;


	// Initializes the character's attributes when the game starts.
	void InitializeAttributes();

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for rolling input */
	void Roll(const FInputActionValue& Value);

	/** Called to stop rolling input */
	void StopRolling(const FInputActionValue& Value);

	/** Handle Jump */
	virtual void Jump() override;

	void DestroyCharacter();
	int Damage;

	int Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Shake")
	TSubclassOf<UCameraShakeBase> HitCameraShake;
			
	void StartAttack(const FInputActionValue& Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UCapsuleComponent* SwordHitbox;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* SwordMesh;

	UPROPERTY(EditAnywhere)
	class UAnimSequence* AttackAnimation;

	UFUNCTION()
	void OnSwordHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|VFX")
	UParticleSystem* ImpactVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|VFX")
	UNiagaraSystem* NiagaraDashVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|SFX")
	USoundCue* SoundCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump|SFX")
	USoundCue* JumpSFX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* AudioComponent;

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere);
	bool bCanRegenStamina = true;
private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> WeaponInventoryWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SlotWidgetClass;

	UUserWidget* WeaponInventoryWidget;
	UUserWidget* SlotWidget;

	void TryPickupWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UWeaponInventoryComponent* WeaponInventory;

	// Timer handle for stamina/mana regen

	
	UPROPERTY(EditAnywhere, Category = "Attributes")
	float RegenDelay = 0.1f;

	// Regeneration amount per tick
	UPROPERTY(EditAnywhere, Category = "Attributes")
	float StaminaRegenRate = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	float ManaRegenRate = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	float RegenInterval = 1.0f; // Every second

	// Function to restore stamina/mana
	void RegenerateAttributes(float DeltaTime);
	void StartUtilityRegen();
	void RegenerateUtility();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable)
	void EndAttack();

	UFUNCTION(BlueprintCallable)
	void ExplosionAttack();

	UFUNCTION(BlueprintCallable)
	void DestroyExplosionHitbox();

	UFUNCTION()
	void ExpandExplosionHitbox();

	UFUNCTION()
	void OnExplosionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void SwitchToNextWeapon();

	UPROPERTY(EditAnywhere, Category = "Explosion Attack")
	float MaxExplosionRadius = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Explosion Attack")
	float ExpansionTime = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Explosion Attack")
	float ExplosionForce = 1500.0f;

	UFUNCTION(BlueprintCallable)
	void EnableHitbox();

	UFUNCTION(BlueprintCallable)
	void DisableHitbox();

	void TogglePauseMenu();
	void ToggleGameOver();

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UPauseMenuWidget> PauseMenuClass;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UGameOverWidget> GameOverClass;

	UPROPERTY()
	UPauseMenuWidget* PauseMenuInstance;
	UPROPERTY()
	UGameOverWidget* GameOverInstance;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool bIsAttacking;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool bIsHoldingAttack;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool bCanJump;

	/** When true, player wants to roll */
	UPROPERTY(BlueprintReadOnly, Category = Character)
	uint8 bPressedRoll : 1;

	UPROPERTY(EditAnywhere, Category = Player)
	float DashSpeed = 1500.f;

	UPROPERTY(EditAnywhere, Category = Player)
	float DashCoolDown;

	UPROPERTY(EditAnywhere, Category = Player)
	float DashDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	bool bIsDashing = false;

private:
	UPROPERTY()
	USphereComponent* ExplosionHitbox;

	FTimerHandle DestroyHitboxTimerHandle;

	FVector InitialLocation;  // Stores the location before attack starts
	FTimerHandle AttackTimer;
	// Function to handle attribute changes

	// Function to handle changes in health attributes
	void HandleHealthChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(EditAnywhere, Category = Player)
	bool bCanDash;

	UPROPERTY(EditAnywhere, Category = Player)
	float DashBreakFriction;

	UPROPERTY(EditAnywhere, Category = Player)
	float DashFriction;

	// Specifies which properties should be replicated over the network
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere)
	bool bIsDead = false;

private:
	

	float DefaultFriction;
	float DefaultWalkSpeed;
	float DefualtBreakFriction;

	FTimerHandle RegenTimerHandle;
	FTimerHandle DeathTimerHandle;
	FTimerHandle StaminaRegenTimerHandle;
	FTimerHandle DashTimerHandle;
	FTimerHandle CoolDownTimerHandle;
};

