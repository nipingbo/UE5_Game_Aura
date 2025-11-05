#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UUserWidget;
class UAuraUserWidget;
class UOverlayWidgetController;
class APlayerController;
class APlayerState;
class UAbilitySystemComponent;
class UAttributeSet;
struct FWidgetControllerParams;

UCLASS()
class AURA_API AAuraHUD : public AHUD
{
    GENERATED_BODY()

public:

    AAuraHUD();

    UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);

    // 初始化 Overlay（角色层会调用）
    void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

    // 查询是否已初始化（供外部判断）
    bool IsOverlayInitialized() const { return bOverlayInitialized; }

protected:
    // Overlay Widget 的类（在编辑器中设置）
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> OverlayWidgetClass;

    // Overlay Widget Controller 类
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

private:
    // Overlay Widget Controller 实例
    UPROPERTY()
    UOverlayWidgetController* OverlayWidgetController = nullptr;

    // Overlay Widget 实例
    UPROPERTY()
    UAuraUserWidget* OverlayWidget = nullptr;

    // 是否已经初始化过 Overlay（防重复创建）
    bool bOverlayInitialized = false;
};
