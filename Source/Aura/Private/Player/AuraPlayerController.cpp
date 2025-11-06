// Copyright Raymond


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "AbilitySystemComponent.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	//check(Subsystem) if we use check(Subsystem) statement here, it is not correct
	//Because in the multiplayer, on client machine, other clients' Subsystem could be null
	
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = CurrentActor;
	CurrentActor = CursorHit.GetActor();

	/**
	 * Line trace from cursor. There are several scenarios:
	 *  A. LastActor is null && CurrentActor is null
	 *		- Do nothing
	 *	B. LastActor is null && CurrentActor is valid
	 *		- Highlight ThisActor
	 *	C. LastActor is valid && CurrentActor is null
	 *		- UnHighlight LastActor
	 *	D. Both actors are valid, but LastActor != CurrentActor
	 *		- UnHighlight LastActor, and Highlight CurrentActor
	 *	E. Both actors are valid, and are the same actor
	 *		- Do nothing
	 */
	if (LastActor == nullptr)
	{
		if (CurrentActor != nullptr)
		{
			//case B
			CurrentActor->HighLightActor();
		}
		else
		{
			//case A do nothing
		}
	}
	else
	{
		if (CurrentActor == nullptr)
		{
			//case C
			LastActor->UnHighLightActor();
		}
		else
		{
			if (LastActor != CurrentActor)
			{
				//case D
				CurrentActor->HighLightActor();
				LastActor->UnHighLightActor();
			}
			else
			{
				//case E do nothing
			}
		}
	}
	
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	if (APawn* ControlledPawn = GetPawn())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	UE_LOG(LogTemp, Warning, TEXT("AAuraPlayerController::OnRep_PlayerState: PC=%s PS=%s IsLocal=%d NetMode=%d PID=%u"),
		*GetNameSafe(this),
		*GetNameSafe(PlayerState),
		IsLocalController(),
		(int)GetNetMode(),
		(uint32)FPlatformProcess::GetCurrentProcessId());

	if (!IsLocalController())
	{
		return;
	}

	AAuraPlayerState* APS = Cast<AAuraPlayerState>(PlayerState);
	if (!APS)
	{
		UE_LOG(LogTemp, Warning, TEXT("AAuraPlayerController::OnRep_PlayerState: PlayerState is not AAuraPlayerState"));
		return;
	}

	UAbilitySystemComponent* ASC = APS->GetAbilitySystemComponent();
	if (ASC)
	{
		if (AAuraHUD* HUD = Cast<AAuraHUD>(GetHUD()))
		{
			HUD->InitOverlay(this, APS, ASC, APS->GetAttributeSet());
			UE_LOG(LogTemp, Warning, TEXT("AAuraPlayerController::OnRep_PlayerState: Called HUD->InitOverlay"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AAuraPlayerController::OnRep_PlayerState: HUD not ready yet"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AAuraPlayerController::OnRep_PlayerState: ASC not ready yet, waiting for PlayerState OnRep"));
	}
}

