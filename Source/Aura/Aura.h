// Copyright Raymond

#pragma once

#include "CoreMinimal.h"

#define CUSTOM_DEPTH_RED 250
#define ECC_Projectile ECollisionChannel::ECC_GameTraceChannel1

#if !UE_BUILD_SHIPPING
	#define AURA_SCREEN_DEBUG(Format, ...) \
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(Format, ##__VA_ARGS__))
#else
	#define AURA_SCREEN_DEBUG(Format, ...)
#endif

