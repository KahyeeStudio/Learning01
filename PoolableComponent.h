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
	/** 默认为 AlwaysSpawn。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESpawnActorCollisionHandlingMethod CollisionHandlingMethodOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	/** 默认为 MultiplyWithRoot。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESpawnActorScaleMethod TransformScaleMethodOverride = ESpawnActorScaleMethod::MultiplyWithRoot;	
};

/** 蓝图可见：用于传递“从池里取出时”的激活选项*/
USTRUCT(BlueprintType) 
struct FPoolSpawnOptions
{
	GENERATED_BODY() // UE 反射必须宏

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // 是否显示 Actor
	bool bUnhideActor = true; // 默认：取出时显示

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // 是否启用 Actor Tick
	bool bEnableActorTick = true; // 默认：取出时允许 Tick

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // 是否启用 Actor 碰撞
	bool bEnableCollision = true; // 默认：取出时开碰撞

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // 是否启用组件 Tick（统一开关）
	bool bEnableComponentTick = true; // 默认：取出时组件也能 Tick

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // 是否在取出时“清理物理速度”（避免上一次残留）
	bool bResetPhysicsVelocity = true; // 默认：清速度

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // 是否在取出时“激活粒子/特效组件”
	bool bActivateFXComponents = true; // 默认：激活

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // 是否在取出时“激活音频组件”
	bool bActivateAudioComponents = true; // 默认：激活

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // 是否把 Transform 设置为传入值
	bool bSetTransform = true; // 默认：设置 Transform
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FScPoolSimpleEvent); // 蓝图可绑定的简单事件委托

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class A1PROJECTSCAVENGER_API UPoolableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UPoolableComponent();
	
	/** 蓝图可调用：主动归还（常用于子弹飞完/特效结束）*/
	UFUNCTION(BlueprintCallable)
	void ReturnToPool();

	/** 蓝图可调用：是否处于池内（休眠状态）*/
	UFUNCTION(BlueprintCallable)
	bool IsInPool() const {return bInPool;}

	/** 蓝图可调用：设置自动回收时间（<=0 表示不自动回收）*/
	UFUNCTION(BlueprintCallable)
	void SetAutoReturnTime(const float InSeconds);

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
	
protected:
	
	virtual void BeginPlay() override;
	
private:

	UPROPERTY() // 反射属性：避免 GC 误清（其实组件跟随 Actor，不强求，但建议规范）
	TObjectPtr<AActor> CachedOwnerActor; // 缓存 Owner Actor 指针

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

	void ClearAutoReturnTimer(); // 清理自动回收定时器

	void StartAutoReturnTimer(); // 启动自动回收定时器

	void ApplyDeactivateStateToActor(); // 统一把 Actor 变成“池内休眠态”

	void ApplyActivateStateToActor(const FTransform& InTransform, const FPoolSpawnOptions& InOptions); // 统一把 Actor 变成“池外活跃态”
};