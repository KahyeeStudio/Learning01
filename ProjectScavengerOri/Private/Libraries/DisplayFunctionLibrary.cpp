// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "Libraries/DisplayFunctionLibrary.h"
#include "Framework/Application/SlateApplication.h"
#include "HAL/PlatformApplicationMisc.h" // 需要此头文件来访问FDisplayMetrics。
#include "Engine/Engine.h"

// 获取显示器总数。
int32 UDisplayFunctionLibrary::GetMonitorCount()
{
	FDisplayMetrics DisplayMetrics;
	FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);
	return DisplayMetrics.MonitorInfo.Num();
}

// 获取当前显示器索引。
int32 UDisplayFunctionLibrary::GetCurrentMonitorIndex()
{
	if (!GEngine || !GEngine->GameViewport)
	{
		return -1;
	}

	TSharedPtr<SWindow> GameWindow = GEngine->GameViewport->GetWindow();
	if (!GameWindow.IsValid())
	{
		return -1;
	}

	FDisplayMetrics DisplayMetrics;
	FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);

	const FVector2D WindowPosition = GameWindow->GetPositionInScreen();

	for (int32 i = 0; i < DisplayMetrics.MonitorInfo.Num(); ++i)
	{
		// 直接使用 WorkArea 的实际类型 FPlatformRect。
		const FPlatformRect& CurrentWorkArea = DisplayMetrics.MonitorInfo[i].WorkArea;

		if (WindowPosition.X >= CurrentWorkArea.Left && WindowPosition.X < CurrentWorkArea.Right &&
			WindowPosition.Y >= CurrentWorkArea.Top && WindowPosition.Y < CurrentWorkArea.Bottom)
		{
			return i;
		}
	}

	return -1;
}

// 获取当前游戏窗口所在的显示器的原生分辨率(最大支持的分辨率)。
bool UDisplayFunctionLibrary::GetCurrentMonitorNativeResolution(int32& OutWidth, int32& OutHeight)
{
	// 设置默认返回值，以防失败。
	OutWidth = 0;
	OutHeight = 0;

	if (!GEngine || !GEngine->GameViewport)
	{
		return false;
	}

	// 获取游戏窗口的引用。
	TSharedPtr<SWindow> Window = GEngine->GameViewport->GetWindow();
	if (!Window.IsValid())
	{
		return false;
	}

	// 获取窗口左上角在整个桌面上的坐标。
	const FVector2D WindowPosition = Window->GetPositionInScreen();

	// 获取所有显示器的信息。
	FDisplayMetrics Metrics;
	// 注意：为与您的其他代码保持一致，这里也使用 FSlateApplication 的方法
	FSlateApplication::Get().GetDisplayMetrics(Metrics);

	// 遍历所有显示器，找到窗口所在的那个。
	for (const FMonitorInfo& Monitor : Metrics.MonitorInfo)
	{
		// FPlatformRect 没有 .Contains() 方法, 需要手动判断。
		const FPlatformRect& CurrentWorkArea = Monitor.WorkArea;
		if (WindowPosition.X >= CurrentWorkArea.Left && WindowPosition.X < CurrentWorkArea.Right &&
			WindowPosition.Y >= CurrentWorkArea.Top && WindowPosition.Y < CurrentWorkArea.Bottom)
		{
			// 找到了！返回这个显示器的原生分辨率。
			OutWidth = Monitor.NativeWidth;
			OutHeight = Monitor.NativeHeight;
			return true; // 成功，并退出函数。
		}
	}

	// 如果循环结束还没找到，则返回失败。
	return false;
}

// 移动窗口到指定显示器。
bool UDisplayFunctionLibrary::MoveWindowToMonitor(const int32 TargetIndex)
{
	if (!GEngine || !GEngine->GameViewport)
	{
		return false;
	}

	TSharedPtr<SWindow> GameWindow = GEngine->GameViewport->GetWindow();
	if (!GameWindow.IsValid())
	{
		return false;
	}

	FDisplayMetrics DisplayMetrics;
	FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);

	if (!DisplayMetrics.MonitorInfo.IsValidIndex(TargetIndex))
	{
		return false;
	}

	// 直接使用 WorkArea 的实际类型 FPlatformRect。
	const FPlatformRect& TargetWorkArea = DisplayMetrics.MonitorInfo[TargetIndex].WorkArea;

	const FVector2D WindowSize = GameWindow->GetSizeInScreen();

	const FVector2D TargetCenter(
		(TargetWorkArea.Left + TargetWorkArea.Right) / 2.0f,
		(TargetWorkArea.Top + TargetWorkArea.Bottom) / 2.0f
	);
	const FVector2D TargetPosition = TargetCenter - (WindowSize / 2.0f);

	GameWindow->MoveWindowTo(TargetPosition);

	return true;
}