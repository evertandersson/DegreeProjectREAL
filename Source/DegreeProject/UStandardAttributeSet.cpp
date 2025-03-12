// Fill out your copyright notice in the Description page of Project Settings.


#include "UStandardAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

// Initilizes attribute values
UStandardAttributeSet::UStandardAttributeSet()
{
	// Set default values for health attributes
	CurrentHealth.SetBaseValue(100.f);
	CurrentHealth.SetCurrentValue(100.f);

	// Set default values for max health attributes
	MaxHealth.SetBaseValue(100.f);
	MaxHealth.SetCurrentValue(100.f);
}

void UStandardAttributeSet::OnRep_CurrentHealth(const FGameplayAttributeData& OldCurrentHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStandardAttributeSet, CurrentHealth, OldCurrentHealth);
}

void UStandardAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldHealthMax)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStandardAttributeSet, MaxHealth, OldHealthMax);
}

// Runs before any attribute values changes, to handle validation or clamping
void UStandardAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Clamp the new value of CurrentHealth
	if (Attribute == GetCurrentHealthAttribute())
	{
		const float MaxHealthValue = MaxHealth.GetCurrentValue();
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxHealthValue);
	}
}

// Handles additional logic after a gameplay effect modifies an attribute
void UStandardAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		float NewHealth = FMath::Clamp(CurrentHealth.GetCurrentValue(), 0.0f, MaxHealth.GetCurrentValue());
		SetCurrentHealth(NewHealth);
	}
}

// Define which properties are replicated over the network
void UStandardAttributeSet::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UStandardAttributeSet, CurrentHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UStandardAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}