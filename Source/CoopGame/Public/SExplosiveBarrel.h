// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"

class UStaticMeshComponent;
class USHealthComponent;

UCLASS()
class COOPGAME_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Components")
	UStaticMeshComponent* MeshComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Components")
    USHealthComponent* HealthComp;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UFUNCTION()
    void OnHealthChanged(USHealthComponent* HealthCom, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
};
