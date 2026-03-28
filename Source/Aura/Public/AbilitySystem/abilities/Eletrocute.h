// Copyright Raymond

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/abilities/AuraBeamSpell.h"
#include "Eletrocute.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UEletrocute : public UAuraBeamSpell
{
	GENERATED_BODY()
	
public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
};
