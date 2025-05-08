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
#include "Components/SphereComponent.h"
#include "Damagable.h"
#include "IHasGroundPos.h"

#include "GrapplingComponent.h"
#include "MantleComponent.h"
#include "WeaponHolderComponent.h"

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
class UGrapplingComponent;
class UMantleComponent;
class ANPC;
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
class ADegreeProjectCharacter : public ACharacter, public IAbilitySystemInterface, public IDamagable, public IIHasGroundPos
{
	GENERATED_BODY()

	#pragma region COMPOENTS
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	#pragma endregion

	#pragma region PLAYER INPUT AND MAPPINGCONTEXT

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
#pragma endregion

public:
	ADegreeProjectCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Implement the interface method to return the Ability System
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Blueprint event to handle health changes and update the UI
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags);

	#pragma region INTERFACE FUNCTIONS

	// Implement the interface function correctly
	virtual void TakeDamage_Implementation(UAbilitySystemComponent* AbilitySystem) override;

	void HandleDeath_Implementation();

	FVector GetGroundPos_Implementation();
	void SetGroundPos();
	FVector GroundPos;

	void ToggleCanJump_Implementation(bool CanJump);

	void ToggleIsMidAir_Implementation(bool bIsMidAir);
	
	void ToggleCanClimb_Implementation(bool bCanClimb);

	virtual void IsInStorm_Implementation(bool bEnable) override;

	void LaunchCharacterInDirection_Implementation(FVector Direction, bool bIsStorm);

	void SetRotationBeforeRoll_Implementation();

	void ConfirmGrappleHit_Implementation();

	void LaunchGrappleHook_Implementation();

	void StandStillForGrappleHook_Implementation(bool bEndAbility);

	UWeaponHolderComponent* GetWeaponHolderComponent_Implementation();

#pragma endregion

	#pragma region TOGGLE AIRBOURNE AND GRAPPLE DELAY FUNCTIONS
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ToggleIsAirbourne();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void CanDisableGrappleDelay();
#pragma endregion

	#pragma region MANA AND STAMINA REGENERATION

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxMana = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxStamina = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxStat = 99; //Change later if needed

	UPROPERTY(EditAnywhere, Category = "Attributes")
	float RegenDelay = 0.1f;
	UPROPERTY(EditAnywhere, Category = "Attributes")
	float StaminaRegenRate = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Attributes")
	float ManaRegenRate = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Attributes")
	float RegenInterval = 1.0f;

#pragma endregion

	#pragma region DASH FUNCTION
	//Dash mechanic
	void Dash();
	void StopDash();
	void ResetDashCoolDown();
#pragma endregion

	#pragma region GET/GIVE DEFAULT ABILITIES AND EFFECTS
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void GiveDefualtAbilities();
	UStandardAttributeSet* GetAttributeSet();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;
#pragma endregion

	#pragma region COMBAT

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer CancelAttacks;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer CancelAttacksDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UCapsuleComponent* SwordHitbox;

	UFUNCTION(BlueprintCallable, Category = "Attack Combo")
	void TriggerCombo();

	UFUNCTION()
	void UpdateAttackAnims();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool bIsHoldingAttack;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool bCanAttack;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool bIsJumpAttackMidAir = false;

	UFUNCTION()
	void SwitchToNextWeapon();

#pragma endregion

protected:
	#pragma region PROTECTED COMPONENTS
	// Ability System Component that manages attributes and effects.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", Replicated, meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent;
	// Attribute Set that stores and manages health and other attributes for replication.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes", Replicated, meta = (AllowPrivateAccess = "true"))
	UStandardAttributeSet* AttributeSet;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UGrapplingComponent* GrapplingComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMantleComponent* MantleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWeaponHolderComponent* WeaponHolderComponent;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* SwordMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Anims")
	TArray<UAnimMontage*> CombatAnims;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickUp|Anims")
	class UAnimMontage* PickUpAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|VFX")
	UNiagaraSystem* NiagaraDashVFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump|SFX")
	USoundCue* JumpSFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|SFX")
	USoundCue* EquipItemSFX;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Audio")
	UAudioComponent* AudioComponent;

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
#pragma endregion

	#pragma region PROTECTED FUNCTIONS
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

	void PlayerKilled();
#pragma endregion

	#pragma region PROTECTED VARIABLES
	int Damage;
	int Health;

	UPROPERTY(EditAnywhere);
	bool bCanRegenStamina = true;
#pragma endregion

private:

	#pragma region PRIVATE INVENTORY/PICKUP FUNCTIONS
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> WeaponInventoryWidgetClass;

	UUserWidget* WeaponInventoryWidget;

	void TryPickupWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UWeaponInventoryComponent* WeaponInventory;

#pragma endregion

	#pragma region PRIVATE ATTRIBUTE REGENERATION
	
	void RegenerateAttributes(float DeltaTime);
	void StartUtilityRegen();
	void RegenerateUtility();
#pragma endregion

public:
	#pragma region CAMERA COMPONENTS
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
#pragma endregion

	#pragma region TOGGLE OVERLAY SCREENS

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

#pragma endregion

	#pragma region BOOLEANS AND FLOATS FOR JUMP AND DASH
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool bCanJump;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool bDidJump;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool bIsDead = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	bool bIsDashing = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	bool bCanDash;

	/** When true, player wants to roll */
	UPROPERTY(BlueprintReadOnly, Category = Character)
	uint8 bPressedRoll : 1;

	UPROPERTY(EditAnywhere, Category = Player)
	float DashSpeed = 1500.f;
	UPROPERTY(EditAnywhere, Category = Player)
	float DashCoolDown;
	UPROPERTY(EditAnywhere, Category = Player)
	float DashDuration;
#pragma endregion

private:
	#pragma region SOME USELESS FUNCTIONS
	void HandleHealthChanged(const FOnAttributeChangeData& Data);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma endregion
private:
	#pragma region PRIVATE VARIABLES
	float DefaultFriction;
	float DefaultWalkSpeed;
	float DefualtBreakFriction;

	UPROPERTY(EditAnywhere, Category = Player)
	float DashBreakFriction;

	UPROPERTY(EditAnywhere, Category = Player)
	float DashFriction;

	FVector InitialLocation;

	FTimerHandle RegenTimerHandle;
	FTimerHandle DeathTimerHandle;
	FTimerHandle StaminaRegenTimerHandle;
	FTimerHandle DashTimerHandle;
	FTimerHandle CoolDownTimerHandle;
#pragma endregion
};