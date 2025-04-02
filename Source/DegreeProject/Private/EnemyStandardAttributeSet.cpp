// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyStandardAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UEnemyStandardAttributeSet::UEnemyStandardAttributeSet()
{
	CurrentHealthAI.SetBaseValue(100.f);
	CurrentHealthAI.SetCurrentValue(100.f);

	MaxHealthAI.SetBaseValue(100.f);
	MaxHealthAI.SetCurrentValue(100.f);
}

void UEnemyStandardAttributeSet::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UEnemyStandardAttributeSet, CurrentHealthAI, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEnemyStandardAttributeSet, MaxHealthAI, COND_None, REPNOTIFY_Always);

}

void UEnemyStandardAttributeSet::OnRep_CurrentHealth(const FGameplayAttributeData& OldCurrentHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyStandardAttributeSet, CurrentHealthAI, OldCurrentHealth);
}

void UEnemyStandardAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyStandardAttributeSet, MaxHealthAI, OldMaxHealth);

}

void UEnemyStandardAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetCurrentHealthAIAttribute())
	{
		const float MaxHealthValue = MaxHealthAI.GetCurrentValue();
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxHealthValue);
	}
}

void UEnemyStandardAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetCurrentHealthAIAttribute())
	{
		float NewHealth = FMath::Clamp(CurrentHealthAI.GetCurrentValue(), 0.0f, MaxHealthAI.GetCurrentValue());
		SetCurrentHealthAI(NewHealth);

		//Add Death here for AI 
		if(NewHealth <= 0.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("AI should die now!"));
			//Handle AI Death
			
		}
	}
}


