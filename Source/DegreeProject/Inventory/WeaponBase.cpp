// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

UTexture2D* AWeaponBase::GetWeaponIcon() const
{
	return WeaponIcon;  // WeaponIcon would be a UTexture2D* variable in your weapon class
}

void AWeaponBase::ActivateCombo(int32 ComboIndex, AActor* Instigator)
{
    if (!CombatAnims.IsValidIndex(ComboIndex) || !ComboAbilities.IsValidIndex(ComboIndex))
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid combo index."));
        return;
    }

    if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Instigator))
    {
        TSubclassOf<UComboAbilityBase> AbilityClass = ComboAbilities[ComboIndex];
        if (ASC->TryActivateAbilityByClass(AbilityClass))
        {
            UE_LOG(LogTemp, Log, TEXT("Activated Combo %d from weapon."), ComboIndex);
        }
    }
}

