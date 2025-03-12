// Copyright Epic Games, Inc. All Rights Reserved.

#include "DegreeProjectCharacter.h"

#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "UStandardAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include "Engine/LocalPlayer.h"

#include "MyAbilitySystemComponent.h"
#include "MyDashAbility.h"

#include "Camera/CameraComponent.h"
#include "KnightAnimationClass.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISenseConfig_Sight.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ADegreeProjectCharacter

ADegreeProjectCharacter::ADegreeProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
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
	SwordMesh->SetupAttachment(GetMesh(),FName("SwordSocket"));

	// Initialize the Ability System Component and enable replication
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Initialize the Attribute Set component for managing health and other attributes
	AttributeSet = CreateDefaultSubobject<UStandardAttributeSet>(TEXT("AttributeSet"));


	SetupStimulusSource();
}


// Returns the Ability System Component for this character
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
}

void ADegreeProjectCharacter::InitializeAttributes() 
{
	if (AbilitySystemComponent && AttributeSet)
	{

	}
}

// Handles changes to health and triggers events to update the UI
void ADegreeProjectCharacter::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	float NewHealth = Data.NewValue;
	float OldHealth = Data.OldValue;

	// Calculate the difference in health to find out the change amount
	float DeltaValue = NewHealth - OldHealth;

	// Trigger a Blueprint event to update the health display or UI
	OnHealthChanged(DeltaValue, FGameplayTagContainer());
}

// Specifies which properties of the character should be replicated over the network.
void ADegreeProjectCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADegreeProjectCharacter, AbilitySystemComponent);
	DOREPLIFETIME(ADegreeProjectCharacter, AttributeSet);
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

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

		// Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ADegreeProjectCharacter::StartAttack);

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
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ADegreeProjectCharacter::SetupStimulusSource()
{
	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	if (StimulusSource)
	{
		StimulusSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
		StimulusSource->RegisterWithPerceptionSystem();
	}
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

void ADegreeProjectCharacter::StartAttack(const FInputActionValue& Value)
{
	if (AttackAnimation && !bIsAttacking)
	{
		GetMesh()->PlayAnimation(AttackAnimation, false);
		bIsAttacking = true;
		 
	}
}

void ADegreeProjectCharacter::LineTrace()
{
	FVector StartLocation = SwordMesh->GetSocketLocation(FName("Start"));
	FVector EndLocation = SwordMesh->GetSocketLocation(FName("End"));

	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams);

	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1, 0, 1);
	if (HitResult.bBlockingHit)
	{
		AActor* ActorHit = HitResult.GetActor();
		ActorHit->Destroy();
	}
}



void ADegreeProjectCharacter::Dash()
{
	if (!bIsDashing && bCanDash && GetCharacterMovement()->Velocity.Size() > 300.f)// change value if needed
	{
		bIsDashing = true;
		bCanDash = false;
		 
		DefaultFriction = GetCharacterMovement()->GroundFriction;
		DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;


		GetCharacterMovement()->GroundFriction = 0.1f;
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
	if (bIsDashing)
	{
		bIsDashing = false;

		GetCharacterMovement()->GroundFriction = DefaultFriction;
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;

		GetCharacterMovement()->Velocity = FVector::ZeroVector;
		GetCharacterMovement()->StopMovementImmediately();

		GetWorldTimerManager().SetTimer(CoolDownTimerHandle, this, &ADegreeProjectCharacter::ResetDashCoolDown, DashCoolDown, false);

	}
	
}

void ADegreeProjectCharacter::ResetDashCoolDown()
{
	bCanDash = true;
}

void ADegreeProjectCharacter::EndAttack(const FInputActionValue& Value)
{
	bIsAttacking = false;
	UpdateAnimationState(false);
}

void ADegreeProjectCharacter::UpdateAnimationState(bool bIsAttackingAni)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); // Get active AnimBP
	if (AnimInstance)
	{
		UKnightAnimationClass* AnimBP = Cast<UKnightAnimationClass>(AnimInstance);
		if (AnimBP)
		{
			AnimBP->bIsAttacking = bIsAttackingAni; // Set animation state
			UE_LOG(LogTemp, Warning, TEXT("Updated Animation State: %s"), bIsAttackingAni ? TEXT("Attacking") : TEXT("Idle"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to cast to KnightAnimationBlueprint!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Animation Instance found on the Skeletal Mesh!"));
	}
}
