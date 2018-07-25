// Fill out your copyright notice in the Description page of Project Settings.

#include <Public/SHealthComponent.h>
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
    DefaultHealth = 100;
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

    Health = DefaultHealth;

    auto Owner = GetOwner();
    if (Owner)
    {
        Owner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::OnTakeAnyDamageHandler);
    }
}

void USHealthComponent::OnTakeAnyDamageHandler(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
    AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0)
        return;

    Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

    UE_LOG(LogTemp, Log, TEXT("Current Health: %s"), *FString::SanitizeFloat(Health));

    OnHealthChanged.Broadcast(this ,Health, Damage, DamageType, InstigatedBy, DamageCauser);
}
