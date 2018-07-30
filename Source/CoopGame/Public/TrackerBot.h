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

    UPROPERTY(VisibleDefaultsOnly, Category = "Components")
    UStaticMeshComponent* MeshComp;
    
    UPROPERTY(VisibleDefaultsOnly, Category = "Components")
    USHealthComponent* HealthComp;

    UPROPERTY(VisibleDefaultsOnly, Category = "Components")
    USphereComponent* SphereComp;
    
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
    float ExplosionDamage;

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    float ExplosionRange;

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    bool bExplosion;

    FTimerHandle TimerHandle_SelfDamage;

    bool bStartSelfDamage;

    UFUNCTION()
    void SelfDamage();
    
    UFUNCTION()
    void OnHealthChanged(USHealthComponent* HealthCom, float Health, float HealthDelta,
                         const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
