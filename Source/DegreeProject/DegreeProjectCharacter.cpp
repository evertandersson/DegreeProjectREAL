// Copyright Epic Games, Inc. All Rights Reserved.

#include "DegreeProjectCharacter.h"

#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "UStandardAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include "Engine/LocalPlayer.h"

#include "MyAbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "UStandardAttributeSet.h"
#include "MyDashAbility.h"

#include "DegreeProject/UI/PauseMenuWidget.h"
#include "Blueprint/UserWidget.h"

#include "Camera/CameraComponent.h"
#include "KnightAnimationClass.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "InputActionValue.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

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
	SwordMesh->SetupAttachment(GetMesh(),FName("FirstHandSocket"));

	SwordHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SwordHitbox"));
	SwordHitbox->SetupAttachment(SwordMesh);  // Make sure it's attached to the sword mesh properly
	SwordHitbox->SetRelativeLocation(FVector(0, 0, 0));  // Adjust based on the sword's position
	SwordHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SwordHitbox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SwordHitbox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SwordHitbox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// Bind the overlap event
	SwordHitbox->OnComponentBeginOverlap.AddDynamic(this, &ADegreeProjectCharacter::OnSwordHit);

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
	bCanJump = true;

	if (!HitCameraShake)
	{
		UE_LOG(LogTemp, Error, TEXT("Camera shake class is not assigned in the Blueprint."));
	}

	DisableHitbox();
}

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
			//PlayerController->SetInputMode(FInputModeUIOnly());
		}
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

void ADegreeProjectCharacter::OnSwordHit(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OverlappedComponent == SwordHitbox && OtherActor && OtherActor != this)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *OtherActor->GetName());
		if (HitCameraShake)
		{
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientStartCameraShake(HitCameraShake);
			}
		}

		FVector ActorLoc = OtherActor->GetActorLocation(); 
		
		if (ImpactVFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactVFX, ActorLoc, FRotator::ZeroRotator);
		}

		if (NiagaraImpactVFX)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraImpactVFX, ActorLoc, FRotator::ZeroRotator);
		}

		// Destroy the actor after the VFX is spawned
		//OtherActor->Destroy();
	}
}


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

		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &ADegreeProjectCharacter::TogglePauseMenu);

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
	if (!bIsAttacking) // Check if not already attacking
	{
		bIsAttacking = true;
		//GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		//UpdateAnimationState(true);
		UE_LOG(LogTemp, Warning, TEXT("Attack Started!"));

		//EnableHitbox(); // Enable the hitbox when the attack starts
	}
}

void ADegreeProjectCharacter::LineTrace()
{
	//FVector StartLocation = SwordMesh->GetSocketLocation(FName("Start"));
	//FVector EndLocation = SwordMesh->GetSocketLocation(FName("End"));
	//
	//FHitResult HitResult;
	//FCollisionQueryParams TraceParams;
	//TraceParams.AddIgnoredActor(this);
	//
	//GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams);
	//
	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1, 0, 1);
	//if (HitResult.bBlockingHit)
	//{
	//	AActor* ActorHit = HitResult.GetActor();
	//	ActorHit->Destroy();
	//}
}

void ADegreeProjectCharacter::EnableHitbox()
{
	SwordHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	UE_LOG(LogTemp, Warning, TEXT("Hitbox Enabled!"));
}

void ADegreeProjectCharacter::DisableHitbox()
{
	SwordHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UE_LOG(LogTemp, Warning, TEXT("Hitbox Disabled!"));
}

void ADegreeProjectCharacter::Jump()
{
	if (!bCanJump) return;

	Super::Jump();
}

void ADegreeProjectCharacter::Dash()
{
	if (!bIsDashing && bCanDash && GetCharacterMovement()->GetLastInputVector() != FVector::ZeroVector) // change value if needed
	{
		FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(GetCharacterMovement()->GetLastInputVector());
		SetActorRotation(TargetRotation);

		bIsDashing = true;
		bCanDash = false;

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
	if (bIsDashing)
	{
		bIsDashing = false;

		GetCharacterMovement()->GroundFriction = DefaultFriction;
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
		
		GetCharacterMovement()->Velocity = FVector(0, 0, GetCharacterMovement()->Velocity.Z);
		GetCharacterMovement()->BrakingFriction = 0;

		//GetCharacterMovement()->Velocity = FVector::ZeroVector;

		//GetCharacterMovement()->Velocity = FVector::ZeroVector;

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
	//UpdateAnimationState(false);
	DisableHitbox(); // Disable the hitbox when the attack ends
	UE_LOG(LogTemp, Warning, TEXT("Attack Ended!"));
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
