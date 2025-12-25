// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
// 使用结构体需要添加头文件，而不是只做前项声明。
#include "Managers/PoolableComponent.h"
#include "PoolSubsystem.generated.h"

/**
 * 对象池子系统，需搭配对象池组件使用。
 */

USTRUCT() // 单个 Class 的池数据
struct FScActorPool
{
	GENERATED_BODY() // 反射宏

public:

	UPROPERTY() // 存“闲置 Actor”（在池内、休眠态）
	TArray<TWeakObjectPtr<AActor>> InactiveActors; // 用弱指针：避免世界清理时悬挂强引用

	UPROPERTY() // 统计：当前总共创建过多少个
	int32 TotalCreated = 0; // 仅用于 debug/统计
};

UCLASS(BlueprintType)
class A1PROJECTSCAVENGER_API UPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:

	virtual void Deinitialize() override; // 世界结束/切关卡时调用：清理池

	/** 蓝图可调用：预创建/预热（提前生成一批放进池）*/
	UFUNCTION(BlueprintCallable) 
	void Prewarm(TSubclassOf<AActor> ActorClass, int32 Count);
	
	/** 蓝图可调用：从池获取或取出 Actor（没有就生成）*/
	UFUNCTION(BlueprintCallable) 
	AActor* AcquireFromPool(TSubclassOf<AActor> ActorClass, const FPoolSpawnInfo& SpawnInfo, const FPoolSpawnOptions& Options);

	/** 蓝图可调用：归还 Actor 到池*/
	UFUNCTION(BlueprintCallable)
	void ReleaseToPool(AActor* Actor);
	
private:

	UPROPERTY() // 按 Class 分类的池
	TMap<TObjectPtr<UClass>, FScActorPool> Pools; // Key：类；Value：该类的池

	AActor* SpawnNewActor(TSubclassOf<AActor> ActorClass, const FPoolSpawnInfo& SpawnInfo); // 生成新 Actor（内部用）

	UPoolableComponent* FindPoolableComponent(AActor* Actor) const; // 找 Actor 上的 Poolable 组件
};