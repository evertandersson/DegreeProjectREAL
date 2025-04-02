// Fill out your copyright notice in the Description page of Project Settings.


#include "UStandardAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "DegreeProjectCharacter.h"
#include "GameplayEffectExtension.h"

// Initilizes attribute values
UStandardAttributeSet::UStandardAttributeSet() :
	CurrentHealth(100),
	MaxHealth(100),
	Defence(4),
	CurrentMana(100),
	MaxMana(100),
	CurrentStamina(100),
	MaxStamina(100),
	Crit_Chance(4),
	Crit_Damage(60),
	Damage(40)
{
	
	

	// Set default values for health attributes
    CurrentHealth.SetBaseValue(100.f);
	CurrentHealth.SetCurrentValue(100.f);

	// Set default values for max health attributes
	MaxHealth.SetBaseValue(100.f);
	MaxHealth.SetCurrentValue(100.f);

	CurrentMana.SetBaseValue(100.f);
	CurrentMana.SetCurrentValue(100.f);

	// Set default values for max health attributes
	MaxMana.SetBaseValue(100.f);
	MaxMana.SetCurrentValue(100.f);

	CurrentStamina.SetBaseValue(100.f);
	CurrentStamina.SetCurrentValue(100.f);

	// Set default values for max health attributes
	MaxStamina.SetBaseValue(100.f);
	MaxStamina.SetCurrentValue(100.f);

}

void UStandardAttributeSet::AddHealth(float Amount)
{
	if (CurrentHealth.GetCurrentValue() < MaxHealth.GetCurrentValue())
    {
		UE_LOG(LogTemp, Warning, TEXT("HEAL"));
		CurrentHealth.SetCurrentValue(CurrentHealth.GetCurrentValue() + Amount);
	}	
}

void UStandardAttributeSet::RemoveHealth(float Amount)
{
	CurrentHealth.SetCurrentValue(CurrentHealth.GetCurrentValue() - Amount);
}

void UStandardAttributeSet::AddDefence(float Number)
{
	UStandardAttributeSet::Defence.SetCurrentValue(Defence.GetCurrentValue() + Number);
}

void UStandardAttributeSet::ResetDefance(float Number)
{

}

// Define which properties are replicated over the network
void UStandardAttributeSet::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UStandardAttributeSet, CurrentHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UStandardAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UStandardAttributeSet, CurrentMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UStandardAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UStandardAttributeSet, CurrentStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UStandardAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UStandardAttributeSet, Crit_Chance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UStandardAttributeSet, Crit_Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UStandardAttributeSet, Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UStandardAttributeSet, Defence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UStandardAttributeSet, Attack_Speed, COND_None, REPNOTIFY_Always);
}

void UStandardAttributeSet::OnRep_CurrentHealth(const FGameplayAttributeData& OldCurrentHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStandardAttributeSet, CurrentHealth, OldCurrentHealth);
}

void UStandardAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldHealthMax)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStandardAttributeSet, MaxHealth, OldHealthMax);
}

void UStandardAttributeSet::OnRep_CurrentMana(const FGameplayAttributeData& OldCurrentMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStandardAttributeSet, CurrentMana, OldCurrentMana);
}
void UStandardAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldManaMax)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStandardAttributeSet, MaxMana, OldManaMax);
}

void UStandardAttributeSet::OnRep_CurrentStamina(const FGameplayAttributeData& OldCurrentStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStandardAttributeSet, CurrentStamina, OldCurrentStamina);
}
void UStandardAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldStaminaMax)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStandardAttributeSet, MaxStamina, OldStaminaMax);
}

void UStandardAttributeSet::OnRep_Crit_Chance(const FGameplayAttributeData& OldCritChance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStandardAttributeSet, Crit_Chance, OldCritChance);
}

void UStandardAttributeSet::OnRep_Crit_Damage(const FGameplayAttributeData& OldCritDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStandardAttributeSet, Crit_Damage, OldCritDamage);
}

void UStandardAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStandardAttributeSet, Damage, OldDamage);
}

void UStandardAttributeSet::OnRep_Defence(const FGameplayAttributeData& OldDefence)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStandardAttributeSet, Defence, OldDefence);
}

void UStandardAttributeSet::OnRep_Attack_Speed(const FGameplayAttributeData& OldAttackSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStandardAttributeSet, Attack_Speed, OldAttackSpeed);
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
	if (Attribute == GetCurrentManaAttribute()) {
		const float MaxManaValue = MaxMana.GetCurrentValue();
		NewValue = FMath::Clamp(NewValue, 0.f, MaxManaValue);
	}
	if (Attribute == GetCurrentStaminaAttribute()) {
		const float MaxStaminaValue = MaxStamina.GetCurrentValue();
		NewValue = FMath::Clamp(NewValue, 0.f, MaxStaminaValue);
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

		ADegreeProjectCharacter* OwnerCharacter = Cast<ADegreeProjectCharacter>(GetOwningActor());
		if (OwnerCharacter && NewHealth <= 0.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Character should die now!"));
			OwnerCharacter->HandleDeath();
		}
	}

	if (Data.EvaluatedData.Attribute == GetCurrentManaAttribute())
	{
		float NewMana = FMath::Clamp(CurrentMana.GetCurrentValue(), 0.0f, MaxMana.GetCurrentValue());
		SetCurrentMana(NewMana);
	}
	if (Data.EvaluatedData.Attribute == GetCurrentStaminaAttribute())
	{
		float NewStamina = FMath::Clamp(CurrentStamina.GetCurrentValue(), 0.0f, MaxStamina.GetCurrentValue());
		SetCurrentStamina(NewStamina);
	}
}