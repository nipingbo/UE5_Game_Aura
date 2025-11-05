#include "UI/HUD/AuraHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
    // 如果之前已创建 controller，则更新参数（以防止 controller 使用过时的数据）
    if (OverlayWidgetController == nullptr)
    {
        OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
        OverlayWidgetController->SetWidgetControllerParams(WCParams);
        OverlayWidgetController->BindCallbacksToDependencies();
        return OverlayWidgetController;
    }

    // 已存在的 controller 也要刷新参数（确保在多次 InitOverlay 调用时使用最新参数）
    OverlayWidgetController->SetWidgetControllerParams(WCParams);
    return OverlayWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
    // 如果已经初始化过 Overlay，则直接返回，避免重复创建 widget / 绑定事件
    if (bOverlayInitialized) return;

    // 确认必需的 Blueprint 类已经在编辑器中设置
    checkf(OverlayWidgetClass, TEXT("Overlay widget class uninitialized, please fill out BP_AuraHUD"));
    checkf(OverlayWidgetControllerClass, TEXT("Overlay widget controller class uninitialized, please fill out BP_AuraHUD"));

    // 使用 PC 作为 Outer 创建 widget（更合适于拥有者/网络上下文）
    UUserWidget* Widget = CreateWidget<UUserWidget>(PC ? PC : GetWorld(), OverlayWidgetClass);
    OverlayWidget = Cast<UAuraUserWidget>(Widget);
    if (!OverlayWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("AAuraHUD::InitOverlay: Failed to cast created widget to UAuraUserWidget"));
        return;
    }

    // 创建或获取 OverlayWidgetController，并传入参数
    const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
    UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);
    if (!WidgetController)
    {
        UE_LOG(LogTemp, Warning, TEXT("AAuraHUD::InitOverlay: Failed to get or create OverlayWidgetController"));
        return;
    }

    // 将 controller 关联到 widget（确保 widget 不为 nullptr）
    OverlayWidget->SetWidgetController(WidgetController);

    // 广播初始值（让 controller 把当前数据推到 widget）
    WidgetController->BroadcastInitialValues();

    // 将 widget 添加到视口
    Widget->AddToViewport();

    // 标记已初始化，避免重复创建
    bOverlayInitialized = true;
}