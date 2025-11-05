#include "UI/HUD/AuraHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/Widget/AuraUserWidget.h"
#include "GameFramework/PlayerState.h"
#include "UI/WidgetController/OverlayWidgetController.h"

AAuraHUD::AAuraHUD()
	: OverlayWidgetController(nullptr)
	, OverlayWidget(nullptr)
	, bOverlayInitialized(false)
{
	// 如需在构造时做额外的默认初始化可在这里添加
}

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	// 如果之前未创建 controller，则创建并设置参数（但不在这里绑定回调，避免重复绑定）
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		UE_LOG(LogTemp, Verbose, TEXT("GetOverlayWidgetController: created controller %p (ASC=%p)"),
				OverlayWidgetController, static_cast<const void*>(WCParams.AbilitySystemComponent.Get()));
		return OverlayWidgetController;
	}

	// 已存在的 controller 也要刷新参数（确保在多次 InitOverlay 调用时使用最新参数）
	OverlayWidgetController->SetWidgetControllerParams(WCParams);
	UE_LOG(LogTemp, Verbose, TEXT("GetOverlayWidgetController: created controller %p (ASC=%p)"),
				OverlayWidgetController, static_cast<const void*>(WCParams.AbilitySystemComponent.Get()));
	return OverlayWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	UE_LOG(LogTemp, Warning, TEXT("InitOverlay ENTER PC=%p PCName=%s IsLocal=%d NetMode=%d PID=%u"),
	PC,
	PC ? *GetNameSafe(PC) : TEXT("null"),
	PC ? PC->IsLocalController() : 0,
	(int)GetNetMode(),
	(uint32)FPlatformProcess::GetCurrentProcessId());
	
	// 确保只在本地玩家上创建 UI（避免在 server 或非本地 controller 上创建）
	if (!PC || !PC->IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("InitOverlay skipped: not local PC (PC=%s IsLocal=%d)"), *GetNameSafe(PC), PC ? PC->IsLocalController() : 0);
		return;
	}
	
	// 调试信息，帮助确认时序和指针有效性
	UE_LOG(LogTemp, Warning, TEXT("InitOverlay called: PC=%s PS=%s ASC=%p AS=%p"), *GetNameSafe(PC), *GetNameSafe(PS), ASC, AS);

	// 如果已经初始化过 Overlay，则直接返回（生产环境）；调试时可注释掉此行
	// if (bOverlayInitialized) return;

	// 确认必需的 Blueprint 类已经在编辑器中设置
	checkf(OverlayWidgetClass, TEXT("Overlay widget class uninitialized, please fill out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay widget controller class uninitialized, please fill out BP_AuraHUD"));

	// 使用 PC 作为 Owner 创建 widget（更合适于拥有者/网络上下文）
	UUserWidget* Widget = CreateWidget<UUserWidget>(PC, OverlayWidgetClass);
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
	UE_LOG(LogTemp, Warning, TEXT("InitOverlay: SetWidgetController OverlayWidget=%p Controller=%p"), OverlayWidget, WidgetController);

	// 在这里统一调用 Bind（避免在 GetOverlayWidgetController 与 InitOverlay 两地都 bind 导致重复）
	// 如果 BindCallbacksToDependencies 不是幂等的，请先确保 controller 内部有解绑逻辑
	WidgetController->BindCallbacksToDependencies();
	UE_LOG(LogTemp, Warning, TEXT("InitOverlay: Bound callbacks for Controller=%p (ASC=%p)"), WidgetController, ASC);

	// 广播初始值（让 controller 把当前数据推到 widget）
	WidgetController->BroadcastInitialValues();
	UE_LOG(LogTemp, Warning, TEXT("InitOverlay: BroadcastInitialValues called for Controller=%p"), WidgetController);

	// 将 widget 添加到视口
	Widget->AddToViewport();

	// 标记已初始化，避免重复创建（注意：在某些场景下你可能希望延后设置为 true，直到确认已收到初始属性值）
	bOverlayInitialized = true;
}