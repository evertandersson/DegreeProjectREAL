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

    bIsClimbing = false;
}

float UMantleComponent::MantleTrace(ADegreeProjectCharacter* Player, float Length, bool bFromGrapple)
{
    if (!Player) return 0.0f;

    FVector PlayerPos = Player->GetActorLocation() + FVector(0.f, 0.f, 150.f);
    FVector ForwardOffset = Player->GetActorForwardVector() * Length;

    FVector StartLocation = PlayerPos + ForwardOffset;
    FVector EndLocation = StartLocation - FVector(0.f, 0.f, 200.f);

    constexpr float Radius = 10.f;
    constexpr float MaxMantleAngle = 40.f;

    TArray<AActor*> ActorsToIgnore = { Player };
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray = {
        UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic)
    };

    FHitResult HitResult;
    if (!UKismetSystemLibrary::SphereTraceSingleForObjects(
        this, StartLocation, EndLocation, Radius, ObjectTypesArray, false,
        ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))
    {
        MantleType = Enum_MantleType::NoMantle;
        return 0.0f;
    }

    MantleHeight = HitResult.Location;

    // Check for obstacles in front
    if (IsObstacleInFront(Player))
    {
        MantleType = Enum_MantleType::NoMantle;
        return 0.0f;
    }

    return DetermineMantleHeight(Player, HitResult, MaxMantleAngle, bFromGrapple);
}

bool UMantleComponent::IsObstacleInFront(ADegreeProjectCharacter* Player)
{
    FVector StartLocation = Player->GetActorLocation() + FVector(0.f, 0.f, 130.f);
    FVector EndLocation = StartLocation + (Player->GetActorForwardVector() * 100.f);

    FHitResult HitResult;
    return UKismetSystemLibrary::SphereTraceSingleForObjects(
        this, StartLocation, EndLocation, 20.f,
        { UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic) },
        false, { Player }, EDrawDebugTrace::None, HitResult, true);
}

float UMantleComponent::DetermineMantleHeight(ADegreeProjectCharacter* Player, const FHitResult& HitResult, float MaxAngle, bool bFromGrapple)
{
    UMeshComponent* MeshComponent = Player->GetMesh();
    FVector HeadLocation = MeshComponent->GetSocketLocation("head");
    FVector CalfLocation; 
    if (bFromGrapple)
    {
		CalfLocation = MeshComponent->GetSocketLocation("foot_l");
    }
    else 
    {
        CalfLocation = MeshComponent->GetSocketLocation("calf_l");
    }

    float ImpactAngle = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(HitResult.ImpactNormal, FVector(0, 0, 1.f)));

    if (ImpactAngle < MaxAngle && HitResult.Distance > 0.0f)
    {
        if (MantleHeight.Z > HeadLocation.Z)
        {
            MantleType = Enum_MantleType::Mantle2M;
            return MantleHeight.Z - 120.f;
        }
        if (MantleHeight.Z > CalfLocation.Z)
        {
            MantleType = Enum_MantleType::Mantle1M;
            return MantleHeight.Z - 30.f;
        }
    }

    MantleType = Enum_MantleType::NoMantle;
    return 0.0f;
}


