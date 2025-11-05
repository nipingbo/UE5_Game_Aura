// Copyright Raymond


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "Blueprint/UserWidget.h"

AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 400.f, 0);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// init ability actor info for the server
	InitAbilityActorInfo();
}

void AAuraCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
	// init ability actor info for the client when controller is replicated
	InitAbilityActorInfo();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
}


void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	if (!AuraPlayerState) return;
	
	UAbilitySystemComponent* ASC = AuraPlayerState->GetAbilitySystemComponent();
	if (!ASC) return;

	// 如果 ASC 已经对同一个 Owner/Avatar 初始化过，则跳过重复初始化
	if (ASC->AbilityActorInfo
		&& ASC->AbilityActorInfo->OwnerActor == AuraPlayerState
		&& ASC->AbilityActorInfo->AvatarActor == this)
	{
		// 仍然缓存指针以备后续使用
		AbilitySystemComponent = ASC;
		AttributeSet = AuraPlayerState->GetAttributeSet();
		return;
	}

	// (重新)初始化 actor info：Owner 是 PlayerState，Avatar 是 Character
	ASC->InitAbilityActorInfo(AuraPlayerState, this);

	// 仅在成功 cast 为项目自定义的 AuraASC 时调用子类扩展方法
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(ASC))
	{
		AuraASC->AbilityActorInfoSet();
	}

	AbilitySystemComponent = ASC;
	AttributeSet = AuraPlayerState->GetAttributeSet();

	// 初始化 HUD/Overlay — HUD 自身也应防止重复创建 widget
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			// HUD 会判断自己是否已初始化 Overlay
			if (!AuraHUD->IsOverlayInitialized())
			{
				AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
			}
		}
	}
}
