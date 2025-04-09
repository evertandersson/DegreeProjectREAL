// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDashAbility.h"
#include "GameFramework/Character.h"
#include "DegreeProjectCharacter.h"
#include "AbilitySystemComponent.h"

void UMyDashAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{

	if (ADegreeProjectCharacter* Character = Cast<ADegreeProjectCharacter>(ActorInfo->AvatarActor.Get()))
	{
		FTimerHandle DashTimerHandle;
		Character->Dash();

		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		if (ASC && Character->GetVelocity().X >= 0.1f && Character->bCanDash)
		{
			ASC->ApplyModToAttribute(UStandardAttributeSet::GetCurrentStaminaAttribute(), EGameplayModOp::Additive, -20.f);
		}

		Character->GetWorldTimerManager().SetTimer(DashTimerHandle, FTimerDelegate::CreateLambda([this, Handle, ActorInfo, ActivationInfo, Character]()
			{
				if (Character)
				{
					Character->StopDash();
				}

				EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			}), Character->DashDuration, false);
	}

	//EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	
}
