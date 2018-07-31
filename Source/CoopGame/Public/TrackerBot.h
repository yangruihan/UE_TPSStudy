// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TrackerBot.generated.h"

class UStaticMeshComponent;
class USHealthComponent;
class UMaterialInstanceDynamic;
class UParticleSystem;
class USphereComponent;
class USoundCue;
class UAudioComponent;

UCLASS()
class COOPGAME_API ATrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
    UPROPERTY(VisibleDefaultsOnly, Category = "Components")
    UStaticMeshComponent* MeshComp;
    
    UPROPERTY(VisibleDefaultsOnly, Category = "Components")
    USHealthComponent* HealthComp;

    UPROPERTY(VisibleDefaultsOnly, Category = "Components")
    USphereComponent* SphereComp;

    UPROPERTY(VisibleDefaultsOnly, Category = "Components")
    UAudioComponent* AudioComp;

    UPROPERTY(VisibleDefaultsOnly, Category = "Components")
    USphereComponent* TeammateOverlapSphereComp;

protected:
    FVector GetNextPathPoint();
    
    FVector NextPathPoint;

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    float ReachRequiredDistance;
    
    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    bool bUseVelocityChagne;
    
    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    float MovementForce;
    
    UMaterialInstanceDynamic* MatInstance;

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    UParticleSystem* ExplosionEffect;

    void SelfDestruct();

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    float DefaultExplosionDamage;

    UPROPERTY(BlueprintReadonly, Category = "TrackerBot")
    float ExplosionDamage;

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    float DefaultExplosionRange;

    UPROPERTY(BlueprintReadonly, Category = "TrackerBot")
    float ExplosionRange;

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    bool bExplosion;

    FTimerHandle TimerHandle_SelfDamage;

    bool bStartSelfDamage;

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    USoundCue* SelfDestructSoundEffect;

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    USoundCue* ExplosionSoundEffect;

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    float SelfDamageInterval;

    UFUNCTION()
    void SelfDamage();
    
    UFUNCTION()
    void OnHealthChanged(USHealthComponent* HealthCom, float Health, float HealthDelta,
                         const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
    
    UFUNCTION()
    void OnMeshCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

protected:

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    float MaxPowerValue;

    UPROPERTY(ReplicatedUsing = OnRep_CurrentPowerChanged, BlueprintReadonly, Category = "TrackerBot")
    float CurrentPowerValue;

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    float EachTeammateGivenPower;

    void ChangeCurrentPower(int TeammateCount);

    UFUNCTION()
    void OnRep_CurrentPowerChanged();

    float ChangePowerTimer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
