// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "ComboAbilityBase.h"
#include "WeaponBase.generated.h"

UCLASS()
class DEGREEPROJECT_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UTexture2D* WeaponIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
    TArray<TSubclassOf<UComboAbilityBase>> ComboAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*> CombatAnims;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FString WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	USkeletalMeshComponent* WeaponMesh;

	UFUNCTION(BlueprintCallable, Category = "UI")
	UTexture2D* GetWeaponIcon() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ActivateCombo(int32 ComboIndex, AActor* Instigator);
};
