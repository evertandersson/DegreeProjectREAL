// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "UStandardAttributeSet.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class DEGREEPROJECT_API UStandardAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	

public:
	UStandardAttributeSet();

	void AddHealth(float Amount);

	UPROPERTY(EditAnywhere)
	float addhealth;


	// Health attributes:

	// Declares the CurrentHealth attribute, making it readonly in blueprints and replicated to clients.
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing=OnRep_CurrentHealth)
	FGameplayAttributeData CurrentHealth;
	// Macro that creates helper functions to get, set, and initialize CurrentHealth.
	ATTRIBUTE_ACCESSORS(UStandardAttributeSet, CurrentHealth);

	// Declares the MaxHealth attribute, making it readonly in blueprints and replicated to clients.
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	// Macro that creates helper functions to get, set, and initialize MaxHealth.
	ATTRIBUTE_ACCESSORS(UStandardAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData CurrentMana;
	ATTRIBUTE_ACCESSORS(UStandardAttributeSet, CurrentMana);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UStandardAttributeSet, MaxMana);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData CurrentStamina;
	ATTRIBUTE_ACCESSORS(UStandardAttributeSet, CurrentStamina);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UStandardAttributeSet, MaxStamina);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Crit_Chance;
	ATTRIBUTE_ACCESSORS(UStandardAttributeSet, Crit_Chance);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Crit_Damage;
	ATTRIBUTE_ACCESSORS(UStandardAttributeSet, Crit_Damage);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UStandardAttributeSet, Damage);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Defence;
	ATTRIBUTE_ACCESSORS(UStandardAttributeSet, Defence);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Attack_Speed;
	ATTRIBUTE_ACCESSORS(UStandardAttributeSet, Attack_Speed);

protected:
	// Replication notifies
	// Function called on clients when CurrentHealth changes to update the UI or perform logic.
	UFUNCTION()
	void OnRep_CurrentHealth(const FGameplayAttributeData& OldCurrentHealth);
	// Function called on clients when MaxHealth hcanges to update the UI or perform logic.
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	UFUNCTION()
	virtual void OnRep_CurrentMana(const FGameplayAttributeData& OldMana);
	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMana);
	UFUNCTION()
	virtual void OnRep_CurrentStamina(const FGameplayAttributeData& OldStamina);
	UFUNCTION()
	virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldStamina);
	UFUNCTION()
	virtual void OnRep_Crit_Chance(const FGameplayAttributeData& OldCritChance);
	UFUNCTION()
	virtual void OnRep_Crit_Damage(const FGameplayAttributeData& OldCritDamage);
	UFUNCTION()
	virtual void OnRep_Damage(const FGameplayAttributeData& OldDamage);
	UFUNCTION()
	virtual void OnRep_Defence(const FGameplayAttributeData& OldDefence);
	UFUNCTION()
	virtual void OnRep_Attack_Speed(const FGameplayAttributeData& OldAttackSpeed);

	// Attribute change handling
	// Function that runs before any attribute value changes, allowing clamping or validation.
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	// Gameplay effect execution handling
	// Function called after a gameplay effect modifies an attribute to handle post-modification logic.
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
