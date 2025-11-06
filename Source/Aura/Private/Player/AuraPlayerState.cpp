// Copyright Raymond


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "UI/HUD/AuraHUD.h"
#include "Player/AuraPlayerController.h"
#include "Engine/World.h"
#include "Misc/OutputDeviceNull.h"

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
	UE_LOG(LogTemp, Warning, TEXT("AAuraPlayerState::OnRep_AbilitySystemComponent: PS=%s ASC=%p NetMode=%d PID=%u"),
		*GetNameSafe(this),
		static_cast<const void*>(AbilitySystemComponent.Get()),
		(int)GetNetMode(),
		(uint32)FPlatformProcess::GetCurrentProcessId());

	// 找到本地 PlayerController（更可靠的做法）
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->PlayerState == this)
		{
			if (PC->IsLocalController())
			{
				if (AAuraHUD* HUD = Cast<AAuraHUD>(PC->GetHUD()))
				{
					// 注意：AbilitySystemComponent.Get() 取原始指针
					HUD->InitOverlay(Cast<AAuraPlayerController>(PC), this, AbilitySystemComponent.Get(), AttributeSet.Get());
					UE_LOG(LogTemp, Warning, TEXT("AAuraPlayerState::OnRep_AbilitySystemComponent: Called HUD->InitOverlay on PC=%s"), *GetNameSafe(PC));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("AAuraPlayerState::OnRep_AbilitySystemComponent: HUD not ready yet for PC=%s"), *GetNameSafe(PC));
				}
			}
			break;
		}
	}
}

void AAuraPlayerState::OnRep_AttributeSet()
{
	// 目前主要用于调试/扩展；如果需要也可以在这里触发一次 InitOverlay 或 Refresh
	UE_LOG(LogTemp, Verbose, TEXT("AAuraPlayerState::OnRep_AttributeSet: PS=%s AS=%p NetMode=%d PID=%u"),
		*GetNameSafe(this), static_cast<const void*>(AttributeSet.Get()), (int)GetNetMode(), (uint32)FPlatformProcess::GetCurrentProcessId());
}
