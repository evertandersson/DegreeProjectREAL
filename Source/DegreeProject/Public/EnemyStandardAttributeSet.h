// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "EnemyStandardAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class DEGREEPROJECT_API UEnemyStandardAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UEnemyStandardAttributeSet();


	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_CurrentHealth)
	FGameplayAttributeData CurrentHealthAI;
	// Macro that creates helper functions to get, set, and initialize CurrentHealth.
	ATTRIBUTE_ACCESSORS(UEnemyStandardAttributeSet, CurrentHealthAI);

	// Declares the MaxHealth attribute, making it readonly in blueprints and replicated to clients.
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealthAI;
	// Macro that creates helper functions to get, set, and initialize MaxHealth.
	ATTRIBUTE_ACCESSORS(UEnemyStandardAttributeSet, MaxHealthAI);

protected:

	UFUNCTION()
	void OnRep_CurrentHealth(const FGameplayAttributeData& OldCurrentHealth);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

};
