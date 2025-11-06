// Copyright Raymond


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraPlayerController.h"
#include "UI/HUD/AuraHUD.h"

AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>(TEXT("AttributeSet"));
	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraPlayerState, AbilitySystemComponent);
	DOREPLIFETIME(AAuraPlayerState, AttributeSet);
}

void AAuraPlayerState::OnRep_AbilitySystemComponent()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_AbilitySystemComponent: PS=%s ASC=%p NetMode=%d PID=%u"),
		*GetNameSafe(this), static_cast<const void*>(AbilitySystemComponent.Get()), (int)GetNetMode(), (uint32)FPlatformProcess::GetCurrentProcessId());

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->PlayerState == this && PC->IsLocalController())
		{
			if (AAuraHUD* HUD = Cast<AAuraHUD>(PC->GetHUD()))
			{
				HUD->InitOverlay(Cast<AAuraPlayerController>(PC), this, AbilitySystemComponent.Get(), AttributeSet.Get());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("OnRep_AbilitySystemComponent: HUD not ready on PC=%s"), *GetNameSafe(PC));
			}
			break;
		}
	}
}

void AAuraPlayerState::OnRep_AttributeSet()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_AttributeSet: PS=%s AS=%p NetMode=%d PID=%u"),
		*GetNameSafe(this), static_cast<const void*>(AttributeSet.Get()), (int)GetNetMode(), (uint32)FPlatformProcess::GetCurrentProcessId());
}
