// Copyright Epic Games, Inc. All Rights Reserved.

#include "DegreeProjectCharacter.h"

#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "UStandardAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include "Engine/LocalPlayer.h"

#include "MyAbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "MyDashAbility.h"
#include "NPC.h"

#include "TimerManager.h"

#include "DegreeProject/UI/PauseMenuWidget.h"
#include "DegreeProject/UI/GameOverWidget.h"
#include "Blueprint/UserWidget.h"

#include "Camera/CameraComponent.h"
#include "KnightAnimationClass.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Inventory/ComboAbilityBase.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Actor.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

#include "Inventory/WeaponPickup.h"
#include "Inventory/WeaponInventoryComponent.h"
#include "InputActionValue.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ADegreeProjectCharacter

ADegreeProjectCharacter::ADegreeProjectCharacter()
{
	WeaponInventory = CreateDefaultSubobject<UWeaponInventoryComponent>(TEXT("WeaponInventory"));

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword Mesh"));
	SwordMesh->SetupAttachment(GetMesh(),FName("FirstHandSocket"));

	SwordHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SwordHitbox"));
	SwordHitbox->SetupAttachment(SwordMesh);  // Make sure it's attached to the sword mesh properly
	SwordHitbox->SetRelativeLocation(FVector(0, 0, 0));  // Adjust based on the sword's position
	SwordHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SwordHitbox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SwordHitbox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SwordHitbox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// Initialize the Ability System Component and enable replication
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Initialize Grappling Component
	GrapplingComponent = CreateDefaultSubobject<UGrapplingComponent>(TEXT("GrapplingComponent"));

	// Initialize Mantle Component
	MantleComponent = CreateDefaultSubobject<UMantleComponent>(TEXT("MantleComponent"));

	// Initialize Weapon Holder Component
	WeaponHolderComponent = CreateDefaultSubobject<UWeaponHolderComponent>(TEXT("WeaponHolderComponent"));

	// Initialize the Attribute Set component for managing health and other attributes
	AttributeSet = CreateDefaultSubobject<UStandardAttributeSet>(TEXT("AttributeSet"));

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->SetupAttachment(RootComponent);

	bDidJump = false;
}

UAbilitySystemComponent* ADegreeProjectCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ADegreeProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Initialize attributes like health when game starts
	InitializeAttributes();

	// Bind the function to handle health changes to the delegate in the Ability System Component
	if (AbilitySystemComponent && AttributeSet)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentHealthAttribute()).AddUObject(this, &ADegreeProjectCharacter::HandleHealthChanged);
	}

	if (AbilitySystemComponent)
	{
		for (TSubclassOf<UGameplayAbility>& Ability : DefaultAbilities)
		{
			if (Ability)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, static_cast<int32>(EGASAbilityInputID::Confirm), this));
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 0, static_cast<int32>(EGASAbilityInputID::Cancel), this));
			}
		}
	}

	bCanDash = true;
	bIsDashing = false;
	bCanJump = true;

	if (WeaponInventoryWidgetClass)
	{
		WeaponInventoryWidget = CreateWidget<UUserWidget>(GetWorld(), WeaponInventoryWidgetClass);
		if (WeaponInventoryWidget)
		{
			WeaponInventoryWidget->AddToViewport();
		}
	}
}

void ADegreeProjectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RegenerateAttributes(DeltaTime);
	SetGroundPos();
}

void ADegreeProjectCharacter::SetGroundPos()
{
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation - FVector(0, 0, 500); // 500 units down

	FHitResult HitResult;

	// Define what object types to trace against (e.g., WorldStatic)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	// Actors to ignore
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		StartLocation,
		EndLocation,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true
	);

	if (bHit)
	{
		GroundPos = HitResult.Location; 
	}
}

#pragma region PLAYER INPUTS

//////////////////////////////////////////////////////////////////////////
// Input

void ADegreeProjectCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ADegreeProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADegreeProjectCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADegreeProjectCharacter::Look);

		// Rolling
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Started, this, &ADegreeProjectCharacter::Roll);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Completed, this, &ADegreeProjectCharacter::StopRolling);

		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &ADegreeProjectCharacter::TogglePauseMenu);

		EnhancedInputComponent->BindAction(SwitchWeapon, ETriggerEvent::Triggered, this, &ADegreeProjectCharacter::SwitchToNextWeapon);

		EnhancedInputComponent->BindAction(PickUpWeapons, ETriggerEvent::Triggered, this, &ADegreeProjectCharacter::TryPickupWeapon);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ADegreeProjectCharacter::TriggerCombo);

		//Dashing Ensure the abilitySystemComponent is valid
		if (AbilitySystemComponent && PlayerInputComponent)
		{
			AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds(
				"Confirm",
				"Cancel",
				"EGASAbilityInputID",
				static_cast<int32>(EGASAbilityInputID::Confirm),
				static_cast<int32>(EGASAbilityInputID::Cancel)));
		}

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

void ADegreeProjectCharacter::TryPickupWeapon()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AWeaponPickUp::StaticClass());

	UE_LOG(LogTemp, Warning, TEXT("Overlapping pickups found: %d"), OverlappingActors.Num());

	for (AActor* Actor : OverlappingActors)
	{
		AWeaponPickUp* Pickup = Cast<AWeaponPickUp>(Actor);
		if (Pickup)
		{
			float PickUpPlayRate = 3;
			PlayAnimMontage(PickUpAnim, PickUpPlayRate);
			UE_LOG(LogTemp, Warning, TEXT("Found weapon pickup, attempting to pick up."));
			Pickup->PickupWeapon();
			UpdateAttackAnims();
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("No valid weapon pickups found."));
}

void ADegreeProjectCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ADegreeProjectCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ADegreeProjectCharacter::Roll(const FInputActionValue& Value)
{
	bPressedRoll = true;
}

void ADegreeProjectCharacter::StopRolling(const FInputActionValue& Value)
{
	bPressedRoll = false;
}

void ADegreeProjectCharacter::Jump()
{
	if (!bCanJump) return;

	if (JumpSFX && JumpCurrentCount < JumpMaxCount)
	{
		AudioComponent->SetSound(JumpSFX);
		AudioComponent->Play();
	}
	Super::Jump();
}

void ADegreeProjectCharacter::TriggerCombo()
{
	if (!WeaponInventory)return;

	AWeaponBase* EquippedWeapon = WeaponInventory->GetEquippedWeapon();
	if (!EquippedWeapon) return;

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (TSubclassOf<UGameplayAbility> AbilityClass : EquippedWeapon->ComboAbilities)
		{
			if (AbilityClass)
			{
				ASC->TryActivateAbilityByClass(AbilityClass);
			}
		}
	}
}

void ADegreeProjectCharacter::Dash()
{
	if (!bIsDashing && bCanDash && GetCharacterMovement()->GetLastInputVector() != FVector::ZeroVector) // change value if needed
	{
		FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(GetCharacterMovement()->GetLastInputVector());
		SetActorRotation(TargetRotation);

		bIsDashing = true;
		bCanDash = false;
		TArray<FName> DashSockets = { "DashVFX", "VFX_C", "RightFootVFX", "LeftFootVFX" };
		if (NiagaraDashVFX)
		{
			for (FName SocketName : DashSockets)
			{
				UNiagaraComponent* DashEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(
					NiagaraDashVFX,
					GetMesh(),
					SocketName,
					FVector::ZeroVector,
					FRotator::ZeroRotator,
					EAttachLocation::SnapToTarget, true
				);

				if (DashEffect)
				{
					DashEffect->SetFloatParameter("Lifetime", DashDuration);
				}
			}
		}

		if (DashSFX)
		{
			AudioComponent->SetSound(DashSFX);
			AudioComponent->Play();
		}

		DefaultFriction = GetCharacterMovement()->GroundFriction;
		DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
		DefualtBreakFriction = GetCharacterMovement()->GroundFriction;


		GetCharacterMovement()->GroundFriction = DashFriction;
		GetCharacterMovement()->BrakingFriction = DashBreakFriction;
		GetCharacterMovement()->MaxWalkSpeed = DashSpeed;

		LaunchCharacter(GetActorForwardVector() * DashSpeed, true, false);

		GetWorldTimerManager().SetTimer(DashTimerHandle, this, &ADegreeProjectCharacter::StopDash, DashDuration, false);
	}
	FString SpeedText = FString::Printf(TEXT("Current Speed: %.2f"), GetCharacterMovement()->Velocity.Size());
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, SpeedText);
	}
}

void ADegreeProjectCharacter::StopDash()
{
	if (bIsDashing && !bIsDead)
	{
		bIsDashing = false;
		bCanDash = false;

		GetCharacterMovement()->GroundFriction = DefaultFriction;
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;

		GetCharacterMovement()->Velocity = FVector(0, 0, GetCharacterMovement()->Velocity.Z);
		GetCharacterMovement()->BrakingFriction = 0;

		GetWorldTimerManager().SetTimer(CoolDownTimerHandle, this, &ADegreeProjectCharacter::ResetDashCoolDown, DashCoolDown, false);
	}
}

void ADegreeProjectCharacter::ResetDashCoolDown()
{
	bCanDash = true;
	bIsDashing = false;
}
void ADegreeProjectCharacter::SwitchToNextWeapon()
{
	if (WeaponInventory)
	if (WeaponInventory && !bIsDead) // guard clause
	{
		if (EquipItemSFX)
		{
			AudioComponent->SetSound(EquipItemSFX);
			AudioComponent->Play();
		}
		WeaponInventory->SwitchWeapon(1);
		UpdateAttackAnims();
	}
}

void ADegreeProjectCharacter::UpdateAttackAnims()
{
	if (!WeaponInventory) return;

	AWeaponBase* EquippedWeapon = WeaponInventory->GetEquippedWeapon();
	if (!EquippedWeapon) return;

	CombatAnims = EquippedWeapon->CombatAnims;

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
		{
			if (UComboAbilityBase* ComboAbility = Cast<UComboAbilityBase>(Spec.GetPrimaryInstance()))
			{
				ComboAbility->SetMontage(CombatAnims);
			}
		}
	}
}

#pragma endregion

#pragma region HANDLE IMPLEMENTATIONS
// Functions to handle player input and other events that are callable from blueprints

void ADegreeProjectCharacter::LaunchCharacterInDirection_Implementation(FVector Direction, bool bIsStorm)
{
	if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		GetMesh()->GlobalAnimRateScale = 1.0f;
		GetMesh()->GetAnimInstance()->StopAllMontages(0.2f);
	}

	if (bIsStorm)
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(CancelAttacksDuration);
	}
	else
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(CancelAttacks);
		if (Direction.Size() < 100.f)
		{
			Direction = Direction.GetSafeNormal() * 100.f;
		}
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}

	bool bOverrideXY = !bIsStorm;

	if (!Direction.IsNearlyZero())
	{
		LaunchCharacter(Direction, bOverrideXY, false);
	}

	if (bIsStorm)
	{
		float SpinPower = GetActorRotation().Yaw + (Direction.Z * 0.15f);
		SetActorRotation(FRotator(GetActorRotation().Pitch, SpinPower, GetActorRotation().Roll));
	}
}

void ADegreeProjectCharacter::ToggleCanJump_Implementation(bool CanJump)
{
	bCanJump = CanJump;
}

void ADegreeProjectCharacter::ToggleIsMidAir_Implementation(bool bIsMidAir)
{
	bIsJumpAttackMidAir = bIsMidAir;
}

void ADegreeProjectCharacter::ToggleCanClimb_Implementation(bool bCanClimb)
{
	MantleComponent->bCanClimb = bCanClimb;
}

void ADegreeProjectCharacter::IsInStorm_Implementation(bool bEnable)
{
}

void ADegreeProjectCharacter::SetRotationBeforeRoll_Implementation()
{
	FVector LastInput = GetCharacterMovement()->GetLastInputVector();

	if (LastInput == FVector::ZeroVector)
		return;

	FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(LastInput);

	SetActorRotation(TargetRotation);
}

UWeaponHolderComponent* ADegreeProjectCharacter::GetWeaponHolderComponent_Implementation()
{
	return WeaponHolderComponent;
}

void ADegreeProjectCharacter::ToggleCollision_Implementation(bool bEnable)
{
	if (bEnable)
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	else 
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

FVector ADegreeProjectCharacter::GetGroundPos_Implementation()
{
	return GroundPos;
}

void ADegreeProjectCharacter::TakeDamage_Implementation(UAbilitySystemComponent* AbilitySystem)
{
}

#pragma endregion

#pragma region HANDLE GRAPPLEHOOK

void ADegreeProjectCharacter::ConfirmGrappleHit_Implementation()
{
	ToggleIsAirbourne();
	CanDisableGrappleDelay();
	GrapplingComponent->SetConfirmGrappleValues();

	FTransform ParticleTransform = FTransform(	
		FRotator::ZeroRotator,		
		GrapplingComponent->GrapplePoint,								
		FVector(0.8f, 0.8f, 0.8f));

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), 
		GrapplingComponent->p_GrappleHitImpact, 
		ParticleTransform,
		true,
		EPSCPoolMethod::AutoRelease,
		true);

	if (GrapplingComponent->p_GrappleHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), 
			GrapplingComponent->p_GrappleHitSound, 
			ParticleTransform.GetLocation());
	}

	GetCharacterMovement()->Velocity = FVector(
		GetCharacterMovement()->Velocity.X,
		GetCharacterMovement()->Velocity.Y,
		0.0f);

	LaunchCharacter(FVector(0, 0, 500.0f), false, false);
}

void ADegreeProjectCharacter::LaunchGrappleHook_Implementation() { }

void ADegreeProjectCharacter::StandStillForGrappleHook_Implementation(bool bEndAbility)
{
	if (bEndAbility)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		MantleComponent->bCanClimb = true;
		return;
	}

	GrapplingComponent->bIsRotatingTowardsGrapplePoint = true;
	MantleComponent->bCanClimb = false;

	if (!GetCharacterMovement()->IsFalling())
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	}
}
#pragma endregion

#pragma region HANDLE ATTRIBUTE AND ABILITIES/EFFECTS

void ADegreeProjectCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAttributes();
	GiveDefualtAbilities();
}

void ADegreeProjectCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AbilitySystemComponent)
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAttributes();
}

void ADegreeProjectCharacter::InitializeAttributes()
{
	if (AbilitySystemComponent && DefaultEffect)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultEffect, 1, EffectContext);

		if (SpecHandle.IsValid())
			FActiveGameplayEffectHandle GEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void ADegreeProjectCharacter::GiveDefualtAbilities()
{
	if (AbilitySystemComponent)
		for (TSubclassOf<UGameplayAbility>& StartupAbility : DefaultAbilities)
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility.GetDefaultObject(), 1, 0));
}

UStandardAttributeSet* ADegreeProjectCharacter::GetAttributeSet()
{
	return AttributeSet;
}
#pragma endregion

#pragma region HANDLE PLAYER DEATH

void ADegreeProjectCharacter::HandleDeath_Implementation()
{
	bIsDead = true;

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->DisableMovement();

	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &ADegreeProjectCharacter::PlayerKilled, 2.f, false);
}

void ADegreeProjectCharacter::PlayerKilled()
{
	ToggleGameOver();
}
#pragma endregion

#pragma region TOGGLE UI
void ADegreeProjectCharacter::TogglePauseMenu()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return;

	if (PlayerController->IsPaused())
	{
		if (PauseMenuInstance)
		{
			PauseMenuInstance->RemoveFromParent();
			PauseMenuInstance = nullptr;
		}
		PlayerController->SetPause(false);
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
	else
	{
		// Pause game and show menu
		if (!PauseMenuInstance && PauseMenuClass)
		{
			PauseMenuInstance = CreateWidget<UPauseMenuWidget>(PlayerController, PauseMenuClass);
		}

		if (PauseMenuInstance)
		{
			PauseMenuInstance->AddToViewport();
			PlayerController->SetPause(true);
			PlayerController->SetShowMouseCursor(true);
			PlayerController->SetInputMode(FInputModeUIOnly());
		}
	}
}

void ADegreeProjectCharacter::ToggleGameOver()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return;

	if (PlayerController->IsPaused())
	{
		if (GameOverInstance)
		{
			GameOverInstance->RemoveFromParent();
			GameOverInstance = nullptr;
		}
		PlayerController->SetPause(false);
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
	else
	{
		// Pause game and show Game Over
		if (!GameOverInstance && GameOverClass && bIsDead)
		{
			GameOverInstance = CreateWidget<UGameOverWidget>(PlayerController, GameOverClass);
			if (!GameOverInstance) return;
		}

		if (GameOverInstance)
		{
			GameOverInstance->AddToViewport();
			PlayerController->SetPause(true);
			PlayerController->SetShowMouseCursor(true);
			PlayerController->SetInputMode(FInputModeUIOnly());
		}
	}
}
#pragma endregion

#pragma region UTILLITY REGENERATION
// Regenerate stamina and mana over time

void ADegreeProjectCharacter::RegenerateAttributes(float DeltaTime)
{
	if (!AttributeSet) return;

	if (AttributeSet->GetCurrentStamina() < AttributeSet->GetMaxStamina() && bCanRegenStamina)
	{

		bCanRegenStamina = false;
		GetWorldTimerManager().ClearTimer(StaminaRegenTimerHandle);
		GetWorldTimerManager().SetTimer(StaminaRegenTimerHandle, this, &ADegreeProjectCharacter::StartUtilityRegen, RegenDelay, false);
	}

	if (AttributeSet->GetCurrentMana() < AttributeSet->GetMaxMana() && bCanRegenStamina)
	{
		bCanRegenStamina = false;
		GetWorldTimerManager().ClearTimer(StaminaRegenTimerHandle);
		GetWorldTimerManager().SetTimer(StaminaRegenTimerHandle, this, &ADegreeProjectCharacter::StartUtilityRegen, RegenDelay, false);
	}
}

void ADegreeProjectCharacter::StartUtilityRegen()
{
	bCanRegenStamina = true;
	RegenerateUtility();
}

void ADegreeProjectCharacter::RegenerateUtility()
{
	float NewStamina = FMath::FInterpTo(AttributeSet->GetCurrentStamina(), AttributeSet->GetMaxStamina(), GetWorld()->GetDeltaSeconds(), 1.f);
	AttributeSet->SetCurrentStamina(FMath::CeilToInt(NewStamina));

	float NewMana = FMath::FInterpTo(AttributeSet->GetCurrentMana(), AttributeSet->GetMaxMana(), GetWorld()->GetDeltaSeconds(), 1.f);
	AttributeSet->SetCurrentMana(FMath::CeilToInt(NewMana));
}
#pragma endregion

void ADegreeProjectCharacter::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	float NewHealth = Data.NewValue;
	float OldHealth = Data.OldValue;
	float DeltaValue = NewHealth - OldHealth;

	OnHealthChanged(DeltaValue, FGameplayTagContainer());
}

void ADegreeProjectCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADegreeProjectCharacter, AbilitySystemComponent);
	DOREPLIFETIME(ADegreeProjectCharacter, AttributeSet);

}