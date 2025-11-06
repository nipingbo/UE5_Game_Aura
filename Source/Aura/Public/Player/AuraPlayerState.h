// Copyright Raymond

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AAuraPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
protected:
	// 被复制到客户端后触发回调（客户端在收到指针时会调用）
	UPROPERTY(ReplicatedUsing = OnRep_AbilitySystemComponent)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(ReplicatedUsing = OnRep_AttributeSet)
	TObjectPtr<UAttributeSet> AttributeSet;

	UFUNCTION()
	void OnRep_AbilitySystemComponent();

	UFUNCTION()
	void OnRep_AttributeSet();

	// replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
