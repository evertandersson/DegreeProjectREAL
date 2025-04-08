// Fill out your copyright notice in the Description page of Project Settings.

#include "GEC_DamageExecution.h"
#include "UStandardAttributeSet.h"
#include "AbilitySystemComponent.h"

struct FDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defence);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentHealth);

	FDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UStandardAttributeSet, Damage, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UStandardAttributeSet, Defence, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UStandardAttributeSet, CurrentHealth, Target, false);
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

	float DamageDone = (BaseDamage * Damage) * (1 - (Defence / 100));

	if (DamageDone < 0.f)
	{
		DamageDone = 0.0f;
	}

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().CurrentHealthProperty, EGameplayModOp::Additive, -DamageDone));
}