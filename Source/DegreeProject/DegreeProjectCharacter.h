// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "UStandardAttributeSet.h"
#include "DegreeProjectCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

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

public:
	ADegreeProjectCharacter();
	
	virtual void BeginPlay() override;

	// Implement the interface method to return the Ability System
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Blueprint event to handle health changes and update the UI
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags);

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

	int Damage;
			
	void StartAttack();


	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* SwordMesh;

	UPROPERTY(EditAnywhere)
	class UAnimSequence* AttackAnimation;

protected:
	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable)
	void LineTrace();


	UPROPERTY(BlueprintReadWrite)
	bool bIsAttacking;


	/** When true, player wants to roll */
	UPROPERTY(BlueprintReadOnly, Category = Character)
	uint8 bPressedRoll : 1;

private:
	// Function to handle attribute changes

	// Function to handle changes in health attributes
	void HandleHealthChanged(const FOnAttributeChangeData& Data);

	// Specifies which properties should be replicated over the network
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

