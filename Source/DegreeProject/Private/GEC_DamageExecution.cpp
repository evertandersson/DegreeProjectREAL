// Fill out your copyright notice in the Description page of Project Settings.

#include "GEC_DamageExecution.h"
#include "UStandardAttributeSet.h"
#include "AbilitySystemComponent.h"

struct FDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defence);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Crit_Chance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Crit_Damage);

	FDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UStandardAttributeSet, Damage, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UStandardAttributeSet, Defence, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UStandardAttributeSet, CurrentHealth, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UStandardAttributeSet, Crit_Chance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UStandardAttributeSet, Crit_Damage, Source, false);
	}
};

static const FDamageStatics& DamageStatics() 
{
	static FDamageStatics DmgStatics;
	return DmgStatics;
}

UGEC_DamageExecution::UGEC_DamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().DefenceDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CurrentHealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().Crit_ChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().Crit_DamageDef);
}

void UGEC_DamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetABSC = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* TargetActor = TargetABSC ? TargetABSC->GetAvatarActor() : nullptr;

	UAbilitySystemComponent* SourceABSC = ExecutionParams.GetSourceAbilitySystemComponent();
	AActor* SourceActor = SourceABSC ? SourceABSC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float BaseDamage = FMath::Max<float>(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Effect.Damage")), false, -1.0f), 0.0f);

	float Defence = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenceDef, EvaluationParameters, Defence);

	float Damage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, Damage);

	float CritChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Crit_ChanceDef, EvaluationParameters, CritChance);

	float CritDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Crit_DamageDef, EvaluationParameters, CritDamage);

	bool bIsCriticalHit = FMath::FRand() <= (CritChance / 100.0f);

	float DamageDone = (BaseDamage * Damage) * (1 - (Defence / 100));

	if (bIsCriticalHit)
	{
		DamageDone *= (1 + (CritDamage / 100.0f));

		// Ensure critical damage is greater than 50
		//if (DamageDone < 50.0f)
		//{
		//	DamageDone = 50.f;
		//}

		UE_LOG(LogTemp, Warning, TEXT("Critical Hit! CritChance: %.2f%%, CritDamage: %.2f%%, DamageDone: %.2f"), CritChance, CritDamage, DamageDone);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Normal Hit. CritChance: %.2f%%, DamageDone: %.2f"), CritChance, DamageDone);
	}

	if (DamageDone < 0.f)
	{
		DamageDone = 0.0f;
	}

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().CurrentHealthProperty, EGameplayModOp::Additive, -DamageDone));
}