// Copyright Raymond

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"

class UGameplayEffect;

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraEffectActor();


protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* targetActor, TSubclassOf<UGameplayEffect> GamePlayEffectClass);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;
	
};
