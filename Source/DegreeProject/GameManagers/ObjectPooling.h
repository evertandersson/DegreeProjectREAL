// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PooledObject.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoolData.h"
#include "ObjectPooling.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEGREEPROJECT_API AObjectPooling : public AActor
{
    GENERATED_BODY()

public:
    AObjectPooling();
    ~AObjectPooling();

    static AObjectPooling* GetInstance(UWorld* World);

    UFUNCTION(BlueprintCallable, Category = "Pooling")
    AActor* SpawnFromPool(FName Tag, FVector Location, FRotator Rotation);

    UFUNCTION(BlueprintCallable, Category = "Pooling")
    void DespawnObject(AActor* ObjectToDespawn);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    void InitializePools();
    void OnLevelLoaded(UWorld* LoadedWorld, const UWorld::InitializationValues InitValues);

    static AObjectPooling* Instance;

    UPROPERTY(EditAnywhere, Category = "Pooling")
    TArray<FPoolData> Pools;

    TMap<FName, TQueue<AActor*, EQueueMode::Spsc>*> PoolDictionary;

    UPROPERTY(VisibleAnywhere, Category = "Pooling")
    TArray<AActor*> ActiveActors;

    UFUNCTION()
    void InitializeActor();
};
