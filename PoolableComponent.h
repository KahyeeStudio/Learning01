// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoolableComponent.generated.h"

/**
 * 池化组件，将此组件添加到需要池化的对象中即可使用对象池。
 */

class UPoolSubsystem;

USTRUCT(BlueprintType)
struct FPoolSpawnInfo
{
	GENERATED_BODY()
	
public:

	/** 默认为 Identity，不做任何变换。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Transform = FTransform::Identity;
	/** Owner 的弱指针。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<AActor> Owner = nullptr;
	/** Instigator 的弱指针。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<APawn> Instigator = nullptr;
	/** 池化 Actor 碰撞组件的碰撞开关模式，默认为只参与查询（Query）。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECollisionEnabled::Type> CompCollisionEnabledType = ECollisionEnabled::QueryOnly;
	/** 覆盖生成时的碰撞模式，默认为 AlwaysSpawn。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESpawnActorCollisionHandlingMethod CollisionHandlingMethodOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	/** 覆盖生成时的变换，默认为 MultiplyWithRoot。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESpawnActorScaleMethod TransformScaleMethodOverride = ESpawnActorScaleMethod::MultiplyWithRoot;	
};

/** 蓝图可见：用于传递“从池里取出时”的激活选项*/
USTRUCT(BlueprintType) 
struct FPoolSpawnOptions
{
	GENERATED_BODY()

public:
	/** 是否显示 Actor， 默认：取出时显示。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUnhideActor = true;
	/** 是否把 Transform 设置为传入值， 默认：设置 Transform。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSetTransform = true;
	/** 是否启用 Actor Tick， 默认：取出时允许 Tick。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableActorTick = true;
	/** 是否启用 Actor 碰撞， 默认：取出时开碰撞。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableCollision = true;
	/** 是否启用组件 Tick（统一开关）， 默认：取出时组件也能 Tick。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableComponentTick = true;
	/** 是否在取出时“清理物理速度”（避免上一次残留）， 默认：清速度。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bResetPhysicsVelocity = true;
	/** 是否在取出时“激活粒子/特效组件”， 默认：激活。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bActivateFXComponents = true;
	/** 是否在取出时“激活音频组件”， 默认：激活。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bActivateAudioComponents = true;	
};

/** 蓝图可绑定的简单事件委托，用于绑定对象池组件调用激活和休眠函数时的广播。*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FScPoolSimpleEvent);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class A1PROJECTSCAVENGER_API UPoolableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UPoolableComponent();	

	/** 蓝图可调用：是否处于池内（休眠状态）*/
	UFUNCTION(BlueprintCallable)
	bool IsInPool() const {return bInPool;}	

	/** 蓝图可调用：从池里取出时由子系统调用，激活*/
	UFUNCTION(BlueprintCallable)
	void ActivatePoolActor(const FPoolSpawnInfo& InSpawnInfo, const FPoolSpawnOptions& InOptions);

	/** 蓝图可调用：归还到池时由子系统调用，休眠*/
	UFUNCTION(BlueprintCallable)
	void DeactivatePoolActor();

	/** 蓝图可绑定：取出时触发（重置状态、播放特效用），取出事件*/
	UPROPERTY(BlueprintAssignable)
	FScPoolSimpleEvent OnAcquireFromPool;

	/** 蓝图可绑定：归还时触发（停止逻辑、清理状态用），归还事件*/
	 UPROPERTY(BlueprintAssignable)
	FScPoolSimpleEvent OnReleaseToPool;
	
	/** 
	 * 设置自动回收时间（<=0 表示不自动回收），并启动自动回收计时器。
	 */
	UFUNCTION(BlueprintCallable)
	void SetAutoReturnTime(const float InSeconds);
	
	/** 
	 * 此函数主要作为对象池组件内部自动回收的回调函数使用。
	 * 归还 Actor 一般使用对象池子系统的 ReleaseToPool 函数。
	 */
	UFUNCTION(BlueprintCallable)
	void ReturnToPool();
	
private:

	/** 
	 * 用于记录当前是否在池内的变量，默认不在池内（刚生成时通常是活跃状态）。
	 */
	UPROPERTY() 
	bool bInPool = false;

	/**
	 * 用于记录自动回收时间（秒）的变量，<= 0 表示不自动回收。
	 */
	UPROPERTY(EditDefaultsOnly)
	float AutoReturnTime = 0.0f;

	FTimerHandle AutoReturnTimerHandle; // 定时器句柄（用于到点自动 ReturnToPool）

	/** 启动自动回收定时器*/
	void StartAutoReturnTimer();
	/* 清理自动回收定时器*/
	void ClearAutoReturnTimer();
	
	/** 
	 * 通过对象池组件激活 Actor 的具体实现。
	 * 统一把 Actor 变成“池外活跃态”。
	 */
	void ApplyActivateStateToActor(const FPoolSpawnInfo& InSpawnInfo, const FPoolSpawnOptions& InOptions);
	/** 
	 * 通过对象池组件休眠 Actor 的具体实现。
	 * 统一把 Actor 变成“池内休眠态”。
	 */
	void ApplyDeactivateStateToActor();	
};