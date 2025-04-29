// Fill out your copyright notice in the Description page of Project Settings.


#include "DealDamage.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"


void UDealDamage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Ainmation)
{
	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	FVector Start = MeshComp->GetComponentLocation();
	FVector End = Start + Owner->GetActorForwardVector() * 100.0f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	if (Owner->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			UGameplayStatics::ApplyDamage(HitActor, 20.0f, Owner->GetInstigatorController(), Owner, nullptr);
			UE_LOG(LogTemp, Warning, TEXT("Enemy attack notify triggered on: %s"), *Owner->GetName());
		}
	}
}
