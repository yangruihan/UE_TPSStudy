// Fill out your copyright notice in the Description page of Project Settings.

#include <Public/SHealthComponent.h>
#include "GameFramework/Actor.h"
#include "UnrealNetwork.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
    DefaultHealth = 100;

    SetIsReplicated(true);
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    Health = DefaultHealth;

    if (GetOwnerRole() == ROLE_Authority)
    {
        auto Owner = GetOwner();
        if (Owner)
        {
            Owner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::OnTakeAnyDamageHandler);
        }
    }
}

void USHealthComponent::OnTakeAnyDamageHandler(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
    AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0)
        return;

    Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

    UE_LOG(LogTemp, Log, TEXT("%s Current Health: %s"), *GetOwner()->GetName(), *FString::SanitizeFloat(Health));

    OnHealthChanged.Broadcast(this ,Health, Damage, DamageType, InstigatedBy, DamageCauser);
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(USHealthComponent, Health);
}