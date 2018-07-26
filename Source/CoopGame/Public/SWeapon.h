// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UParticleSystem;
class UCameraShake;

USTRUCT()
struct FHitScanTrace
{
    GENERATED_BODY()

public:

    UPROPERTY()
    FVector_NetQuantize TraceFrom;

    UPROPERTY()
    FVector_NetQuantize TraceTo;
};

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Components")
    USkeletalMeshComponent* MeshComp;

    UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

    /* Damage */

    UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Weapon")
    float BaseDamage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Weapon")
    float CritDamageRate;

    /* Effect */

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadonly, Category = "Weapon")
    FName MuzzleSocketName;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadonly, Category = "Weapon")
    FName TracerTargetParamName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Weapon")
    UParticleSystem* MuzzleEffect;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Weapon")
    UParticleSystem* DefaultImpactEffect;    
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Weapon")
    UParticleSystem* FleshImpactEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Weapon")
    UParticleSystem* TracerEffect;

    void PlayFireEffect(FVector TracerEndPoint);

    UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
    FHitScanTrace HitScanTrace;

    UFUNCTION()
    void OnRep_HitScanTrace();

protected:

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<UCameraShake> CameraShake;

protected:

    /* Fire */

	virtual void Fire();

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerFire();

    FTimerHandle TimeHandle_TimeBetweenShots;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float RPM; // Revolutions per minute

    float TimeBetweenShots;

    float LastShotTime;

public:	

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void StartFire();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void StopFire();
	
};
