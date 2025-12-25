// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "Managers/PoolableComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h" // UPrimitiveComponent（物理速度等）
#include "Particles/ParticleSystemComponent.h" // UParticleSystemComponent（停粒子）
#include "NiagaraComponent.h" // UNiagaraComponent
#include "Components/AudioComponent.h" // UAudioComponent（停音频）
#include "GameFramework/ProjectileMovementComponent.h"
#include "Managers/PoolSubsystem.h"


UPoolableComponent::UPoolableComponent()
{
	// 该组件本身不需要 Tick（减少开销）
	PrimaryComponentTick.bCanEverTick = false;
}

void UPoolableComponent::BeginPlay()
{
	Super::BeginPlay();
	CachedOwnerActor = GetOwner(); // 缓存 Owner Actor 指针
}

void UPoolableComponent::ReturnToPool()
{
	UWorld* World = GetWorld(); // 获取当前世界
	if (!World) // 如果世界无效
	{
		return; // 直接返回
	}

	AActor* OwnerActor = GetOwner(); // 获取 Owner Actor
	if (!IsValid(OwnerActor)) // 如果 Actor 无效
	{
		return; // 返回
	}

	UPoolSubsystem* PoolSubsystem = World->GetSubsystem<UPoolSubsystem>(); // 获取对象池子系统
	if (!PoolSubsystem) // 如果子系统不存在（理论上不会）
	{
		return; // 返回
	}

	PoolSubsystem->ReleaseToPool(OwnerActor); // 归还 Actor 到池
}

void UPoolableComponent::SetAutoReturnTime(const float InSeconds)
{
    AutoReturnTime = InSeconds; // 保存自动回收秒数（<=0 表示不自动回收）

    ClearAutoReturnTimer(); // 先清理旧定时器（避免重复触发）

    if (!bInPool && AutoReturnTime > 0.0f) // 如果当前对象“在池外活跃”且需要自动回收
    {
        StartAutoReturnTimer(); // 立刻启动定时器（让取出后设置寿命也能生效）
    }
}

void UPoolableComponent::ActivatePoolActor(const FPoolSpawnInfo& InSpawnInfo, const FPoolSpawnOptions& InOptions)
{
	bInPool = false; // 标记：不在池内

	ClearAutoReturnTimer(); // 取出时先清理旧的自动回收定时器

	ApplyActivateStateToActor(InSpawnInfo, InOptions); // 应用“活跃态”到 Actor

	OnAcquireFromPool.Broadcast(); // 广播：取出事件（蓝图可绑定）
}

void UPoolableComponent::DeactivatePoolActor()
{
	bInPool = true; // 标记：在池内

	ClearAutoReturnTimer(); // 归还时清理定时器（避免重复触发）

	ApplyDeactivateStateToActor(); // 应用“休眠态”到 Actor

	OnReleaseToPool.Broadcast(); // 广播：归还事件（蓝图可绑定）
}

void UPoolableComponent::ClearAutoReturnTimer()
{
    UWorld* World = GetWorld(); // 获取世界
    if (!World) // 世界无效
    {
        return; // 返回
    }

    World->GetTimerManager().ClearTimer(AutoReturnTimerHandle); // 清理定时器
}

void UPoolableComponent::StartAutoReturnTimer()
{
    UWorld* World = GetWorld(); // 获取世界
    if (!World) // 世界无效
    {
        return; // 返回
    }

    if (AutoReturnTime <= 0.0f) // 如果不需要自动回收
    {
        return; // 返回
    }

    World->GetTimerManager().SetTimer(
        AutoReturnTimerHandle, // 保存句柄
        this, // 回调对象是自己
        &UPoolableComponent::ReturnToPool, // 到点调用 ReturnToPool
        AutoReturnTime, // 延迟秒数
        false // 不循环（只触发一次）
    );
}

void UPoolableComponent::ApplyDeactivateStateToActor()
{
    // 获取 Owner
    AActor* OwnerActor = GetOwner();
    // 检查有效性
    if (!IsValid(OwnerActor)) return;
    // 在游戏中隐藏 Actor（不渲染）
    OwnerActor->SetActorHiddenInGame(true);
    // Actor 层关碰撞// 关闭 Actor 碰撞
    OwnerActor->SetActorEnableCollision(false);
    // 关闭 Actor Tick（省性能）
    OwnerActor->SetActorTickEnabled(false);
    // 清理归属者（Owner）与自定义状态（避免串台）
    OwnerActor->SetOwner(nullptr);
    OwnerActor->SetInstigator(nullptr);
    // 清理 Timer（如果有任何 Timer，一定要清），ClearAllTimersForObject 函数可清掉属于该对象的所有定时器（简单粗暴）
    if (const UWorld* World = GetWorld())
    {World->GetTimerManager().ClearAllTimersForObject(OwnerActor);}

    TArray<UActorComponent*> Components; // 临时数组：存组件
    OwnerActor->GetComponents(Components); // 获取所有组件

    for (UActorComponent* Comp : Components) // 遍历每个组件
    {
        // 组件无效则跳过，继续检查下一个。
        if (!Comp) continue;
        // 关闭组件 Tick
        Comp->SetComponentTickEnabled(false); 
        
        // 如果是投射物移动组件，则进行如下操作：
        if (UProjectileMovementComponent* ProjectileMoveComp = Cast<UProjectileMovementComponent>(Comp))
        {
            // 停运动
            ProjectileMoveComp->StopMovementImmediately();     // 立刻停
            ProjectileMoveComp->Deactivate();                  // 关闭组件（不再 Tick）
        }

        // 尝试转成粒子组件，如果是粒子，则停止粒子播放
        if (UParticleSystemComponent* ParticleSysComp = Cast<UParticleSystemComponent>(Comp)) 
        {ParticleSysComp->DeactivateSystem();}

         // 尝试转成 Niagara 组件，如果是 Niagara，则停止 Niagara
        if (UNiagaraComponent* NiagaraComp = Cast<UNiagaraComponent>(Comp)) 
        {NiagaraComp->Deactivate();}

         // 尝试转成音频组件，如果是音频，则停止声音
        if (UAudioComponent* AudioComp = Cast<UAudioComponent>(Comp))
        {AudioComp->Stop();}

        // 尝试转成 Primitive（碰撞/物理），如果是 Primitive，则进行如下操作：
        if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Comp))
        {
            PrimitiveComp->SetSimulatePhysics(false); // 关闭物理模拟（可避免回池后还在飞）
            PrimitiveComp->SetPhysicsLinearVelocity(FVector::ZeroVector); // 清线速度
            PrimitiveComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector); // 清角速度
            PrimitiveComp->SetVisibility(false, true); // 组件也隐藏（递归子组件）
            PrimitiveComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 组件碰撞关闭
        }
    }
}

void UPoolableComponent::ApplyActivateStateToActor(const FPoolSpawnInfo& InSpawnInfo, const FPoolSpawnOptions& InOptions)
{
    AActor* OwnerActor = GetOwner(); // 获取 Owner
    // 检查有效性
    if (!IsValid(OwnerActor)) return;
    // 设置归属者（Owner）与自定义状态（避免串台）
    OwnerActor->SetOwner(InSpawnInfo.Owner.Get());
    OwnerActor->SetInstigator(InSpawnInfo.Instigator.Get());
    // 如果需要设置 Transform，设置位置/旋转/缩放 
    if (InOptions.bSetTransform)
    {OwnerActor->SetActorTransform(InSpawnInfo.Transform);}
    // 如果需要显示，显示 Actor
    if (InOptions.bUnhideActor)
    {OwnerActor->SetActorHiddenInGame(false);}
    // 设置 Actor Tick
    OwnerActor->SetActorTickEnabled(InOptions.bEnableActorTick);
    // 设置 Actor 碰撞开关
    OwnerActor->SetActorEnableCollision(InOptions.bEnableCollision);

    TArray<UActorComponent*> Components; // 临时数组：存组件
    OwnerActor->GetComponents(Components); // 获取所有组件

    for (UActorComponent* Comp : Components) // 遍历
    {
        // 如果组件无效，则跳过，继续遍历下一个
        if (!Comp) continue;

        Comp->SetComponentTickEnabled(InOptions.bEnableComponentTick); // 设置组件 Tick
        
        // 如果是投射物移动组件，则进行如下操作：
        if (UProjectileMovementComponent* ProjectileMoveComp = Cast<UProjectileMovementComponent>(Comp))
        {
        	// 确保 UpdatedComponent 正确（池化时强烈建议每次都设一次）
        	if (UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(OwnerActor->GetRootComponent()))
        	{ProjectileMoveComp->SetUpdatedComponent(RootPrim);}
        	// 清理上一轮残留（可留可不留，但留着更稳）
        	ProjectileMoveComp->StopMovementImmediately();
        	// 重新给速度（方向来自 InTransform 的旋转）
        	const FVector ForwardDir = InSpawnInfo.Transform.GetRotation().GetForwardVector(); // 当前朝向的前方单位向量
        	const float Speed = (ProjectileMoveComp->InitialSpeed > 0.f) ? ProjectileMoveComp->InitialSpeed : 550.f; // 兜底
        	ProjectileMoveComp->Velocity = ForwardDir * Speed; // 给出速度向量
        	// 激活并重置
        	ProjectileMoveComp->Activate(true);
        }
        
        // 如果是 Primitive 碰撞体，处理碰撞预设。
        if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Comp))
        {
            PrimitiveComp->SetVisibility(true, true); // 显示组件
            if (InOptions.bEnableCollision) // 如果要求开碰撞
            {
                PrimitiveComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 开启查询与物理（可按需改）
            }
            if (InOptions.bResetPhysicsVelocity) // 如果要清速度
            {
                PrimitiveComp->SetPhysicsLinearVelocity(FVector::ZeroVector); // 清线速度
                PrimitiveComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector); // 清角速度
            }
        }

        if (InOptions.bActivateFXComponents) // 如果要求激活特效
        {
            // 如果是粒子特效，则激活粒子
            if (UParticleSystemComponent* ParticleSysComp = Cast<UParticleSystemComponent>(Comp)) 
            {ParticleSysComp->ActivateSystem(true);}
            // 如果是 Niagara，则激活 Niagara
            if (UNiagaraComponent* NiagaraComp = Cast<UNiagaraComponent>(Comp))
            {NiagaraComp->Activate(true);}
        }

        if (InOptions.bActivateAudioComponents) // 如果要求激活音频
        {
            // 如果是音频，则播放音效
            if (UAudioComponent* AudioComp = Cast<UAudioComponent>(Comp))
            {AudioComp->Play();}
        }
    }
	// 最后启动自动回收（如果设置了 AutoReturnTime）
    StartAutoReturnTimer();
}