// Fill out your copyright notice in the Description page of Project Settings.


#include "MantleComponent.h"
#include "Components/CapsuleComponent.h"
#include "DegreeProjectCharacter.h"
#include "Kismet/KismetSystemLibrary.h" 
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UMantleComponent::UMantleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

float UMantleComponent::MantleTrace(ADegreeProjectCharacter* Player, float Length)
{
    FVector PlayerPos = Player->GetActorLocation();
    PlayerPos.Z += 150.f;
    FVector PlayerForward = Player->GetActorForwardVector() * Length;

    FVector StartLocation = PlayerPos + PlayerForward;
    FVector EndLocation = FVector(StartLocation.X, StartLocation.Y, StartLocation.Z - 200.f);

    float Radius = 10.f;

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Player);

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
    ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

    FHitResult HitResult;

    bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
        this,
        StartLocation,
        EndLocation,
        Radius,
        ObjectTypesArray,
        false,
        ActorsToIgnore,
        EDrawDebugTrace::None,
        HitResult,
        true
    );

    if (HitResult.bBlockingHit)
    {
        MantleHeight = HitResult.Location;

        FVector StartLocation2 = Player->GetActorLocation();
        StartLocation2.Z += 130.f;
        
        FVector PlayerForward2 = Player->GetActorForwardVector() * 100.f;
        FVector EndLocation2 = StartLocation2 + PlayerForward2;

        FHitResult HitResult2;

        bool bHit2 = UKismetSystemLibrary::SphereTraceSingleForObjects(
            this,
            StartLocation2,
            EndLocation2,
            20.f,
            ObjectTypesArray,
            false,
            ActorsToIgnore,
            EDrawDebugTrace::None,
            HitResult2,
            true
        );

        if (HitResult2.bBlockingHit)
        {
            MantleType = Enum_MantleType::NoMantle;
            return 0.0f;
        }
        else 
        {
            UMeshComponent* MeshComponent = Player->GetMesh();
            FVector HeadLocation = MeshComponent->GetSocketLocation("head");
            FVector CalfLocation = MeshComponent->GetSocketLocation("calf_l");

            if (MantleHeight.Z > HeadLocation.Z
                && UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(HitResult.ImpactNormal, FVector(0, 0, 1.f))) < 40.f
                && HitResult.Distance > 0.0f)
            {
                MantleType = Enum_MantleType::Mantle2M;
                return MantleHeight.Z - 120.f;
            }
            else if (MantleHeight.Z > CalfLocation.Z
                && UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(HitResult.ImpactNormal, FVector(0, 0, 1.f))) < 40.f
                && HitResult.Distance > 0.0f)
            {
                MantleType = Enum_MantleType::Mantle1M;
                return MantleHeight.Z - 30.f;
            }
            else 
            {
                MantleType = Enum_MantleType::NoMantle;
                return 0.0f;
            }
        }
    }
    else 
    {
        MantleType = Enum_MantleType::NoMantle;
        return 0.0f;
    }
}



