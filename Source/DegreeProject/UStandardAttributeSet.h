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

protected:
	// Replication notifies
	// Function called on clients when CurrentHealth changes to update the UI or perform logic.
	UFUNCTION()
	void OnRep_CurrentHealth(const FGameplayAttributeData& OldCurrentHealth);

	// Function called on clients when MaxHealth hcanges to update the UI or perform logic.
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	// Attribute change handling
	// Function that runs before any attribute value changes, allowing clamping or validation.
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	// Gameplay effect execution handling
	// Function called after a gameplay effect modifies an attribute to handle post-modification logic.
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
