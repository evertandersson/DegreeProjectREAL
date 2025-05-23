// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ANPC::ANPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize the Attribute Set component for managing health and other attributes
	AttributeSet = CreateDefaultSubobject<UStandardAttributeSet>(TEXT("AttributeSet"));

	// Initialize the Ability System Component and enable replication
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	MaxHealth = 40.f;
	Health = MaxHealth;
}

void ANPC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Add all replicated properties here
	DOREPLIFETIME(ANPC, AbilitySystemComponent);
	// Add more DOREPLIFETIME lines for other replicated properties if needed
}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANPC::TakeDamage_Implementation(UAbilitySystemComponent* AbilitySystem, bool bIsJumpAttack)
{

}

void ANPC::JumpToDestination_Implementation(FVector Destination)
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	
	FVector OutLaunchVelocity;
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(),
		OutLaunchVelocity,
		GetActorLocation(),
		FVector(Destination.X, Destination.Y, Destination.Z + 250.f));

	LaunchCharacter(OutLaunchVelocity, true, true);

	FVector DirToDestination = Destination - GetActorLocation();
	DirToDestination.Normalize();

	SetActorRotation(UKismetMathLibrary::MakeRotFromX(DirToDestination));

	GetWorld()->GetTimerManager().SetTimer(ResetCollisionTimerHandle, this, &ANPC::ResetCollision, ResetTimer, false);
}

void ANPC::DealDamageToPlayer_Implementation()
{
}

void ANPC::HandleKnockbackAnim(bool bIsJumpAttack)
{
	GetController()->StopMovement();

	if (!bIsDead)
	{
		if (bIsJumpAttack)
		{
			// Play knockback montage
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance && KnockbackAnim)
			{
				PlayAnimation(AnimInstance, KnockbackAnim);
			}
		}
		else
		{
			UAnimMontage* AnimToPlay = KnockbackAnimations[UKismetMathLibrary::RandomInteger(KnockbackAnimations.Num())];
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance && AnimToPlay)
			{
				PlayAnimation(AnimInstance, AnimToPlay);
			}
		}
	}
}

void ANPC::PlayAnimation(UAnimInstance* AnimInstance, UAnimMontage* AnimMontage)
{
	// Bind the delegate before playing the montage
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ANPC::OnKnockbackMontageEnded);
	AnimInstance->Montage_Play(AnimMontage, 1.0f);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AnimMontage);
}

void ANPC::OnKnockbackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	OnKnockbackEnd.Broadcast();

	AAIController* AICon = Cast<AAIController>(GetController());
	if (AICon)
	{
		UBlackboardComponent* BlackboardComp = AICon->FindComponentByClass<UBlackboardComponent>();
		if (BlackboardComp)
		{
			UBlackboardData* BlackboardAsset = BlackboardComp->GetBlackboardAsset();
			// Now you have the blackboard asset
			BlackboardComp->SetValueAsEnum("Enemy States", 0);

		}
	}
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void ANPC::ResetCollision()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void ANPC::Knockback_Implementation(float KnockbackAmount)
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	FVector PlayerPos = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
	FVector DirToPlayer = PlayerPos - GetActorLocation();
	DirToPlayer.Normalize();

	SetActorRotation(UKismetMathLibrary::MakeRotFromX(DirToPlayer));

	FVector LaunchVel = (GetActorForwardVector() * -500.f + FVector(0, 0, 200.f)) * KnockbackAmount;
	LaunchCharacter(LaunchVel, true, false);
}