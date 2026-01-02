// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DisplayFunctionLibrary.generated.h"

/**
 * 显示器设置相关的函数库。
 */
UCLASS()
class PROJECTSCAVENGER_API UDisplayFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * 获取当前连接的所有显示器的数量。
	 * 返回显示器的总数。
	 */
	UFUNCTION(BlueprintPure, Category = "DisplayFunctionLibrary")
	static int32 GetMonitorCount();

	/**
	 * 获取游戏窗口当前所在的显示器的编号(索引)。
	 * 返回当前显示器的索引，如果找不到窗口则返回-1。
	 */
	UFUNCTION(BlueprintCallable, Category = "DisplayFunctionLibrary")
	static int32 GetCurrentMonitorIndex();

	/**
	 * 获取当前游戏窗口所在的显示器的原生分辨率(最大支持的分辨率)。
	 * OutWidth 输出宽度， OutHeight 输出高度，如果成功获取则返回true，否则返回false。
	 */
	UFUNCTION(BlueprintPure, Category = "DisplayFunctionLibrary")
	static bool GetCurrentMonitorNativeResolution(int32& OutWidth, int32& OutHeight);

	/**
	 * 将游戏窗口移动到指定的显示器。
	 * TargetIndex - 目标显示器的索引 (0代表主显示器)。
	 * 如果移动成功则返回true，否则返回false (例如索引无效)。
	 */
	UFUNCTION(BlueprintCallable, Category = "DisplayFunctionLibrary")
	static bool MoveWindowToMonitor(const int32 TargetIndex);

};