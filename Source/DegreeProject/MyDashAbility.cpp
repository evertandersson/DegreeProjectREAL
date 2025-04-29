// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDashAbility.h"
#include "GameFramework/Character.h"
#include "DegreeProjectCharacter.h"
#include "AbilitySystemComponent.h"

void UMyDashAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{

	if (ADegreeProjectCharacter* Character = Cast<ADegreeProjectCharacter>(ActorInfo->AvatarActor.Get()))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

		// Check if character can dash and is moving
		if (Character->bCanDash && Character->GetVelocity().Size() > 0.1f)
		{
			if (ASC)
			{
				ASC->ApplyModToAttribute(UStandardAttributeSet::GetCurrentStaminaAttribute(), EGameplayModOp::Additive, -20.f);
			}

			Character->Dash();

			FTimerHandle DashTimerHandle;
			Character->GetWorldTimerManager().SetTimer(DashTimerHandle, FTimerDelegate::CreateLambda([this, Handle, ActorInfo, ActivationInfo, Character]()
				{
					if (Character)
					{
						Character->StopDash();
					}
					EndAbility(Handle, ActorInfo, ActivationInfo, true, false);

				}), Character->DashDuration, false);
		}
		else
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		}
	}
	
}
