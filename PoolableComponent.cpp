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
#include "GameObjects/ScProjectileActor.h"
#include "Managers/PoolSubsystem.h"


UPoolableComponent::UPoolableComponent()
{
	// 该组件本身不需要 Tick（减少开销）
	PrimaryComponentTick.bCanEverTick = false;
}

void UPoolableComponent::ActivatePoolActor(const FPoolSpawnInfo& InSpawnInfo, const FPoolSpawnOptions& InOptions)
{
	// 标记：不在池内
	bInPool = false;
	// 取出时先清理旧的自动回收定时器
	ClearAutoReturnTimer();
	// 应用“活跃态”到 Actor
	ApplyActivateStateToActor(InSpawnInfo, InOptions);
	// 广播：取出事件（蓝图可绑定）
	OnAcquireFromPool.Broadcast();
}

void UPoolableComponent::DeactivatePoolActor()
{
	// 标记：在池内
	bInPool = true;
	// 归还时清理定时器（避免重复触发）
	ClearAutoReturnTimer();
	// 应用“休眠态”到 Actor
	ApplyDeactivateStateToActor();
	// 广播：归还事件（蓝图可绑定）
	OnReleaseToPool.Broadcast();
}

void UPoolableComponent::SetAutoReturnTime(const float InSeconds)
{
	// 保存自动回收秒数（<=0 表示不自动回收）
	AutoReturnTime = InSeconds;
	// 先清理旧定时器（避免重复触发）
	ClearAutoReturnTimer();
	// 如果当前对象“在池外活跃”且需要自动回收，立刻启动定时器（让取出后设置寿命也能生效）
	if (!bInPool && AutoReturnTime > 0.0f) 
	{StartAutoReturnTimer();}
}

void UPoolableComponent::ReturnToPool()
{
    // 获取当前世界
    const UWorld* World = GetWorld();
    if (!World) return;
    // 获取 Owner Actor
    AActor* OwnerActor = GetOwner();
    if (!IsValid(OwnerActor)) return;
    // 获取对象池子系统
    UPoolSubsystem* PoolSubsystem = World->GetSubsystem<UPoolSubsystem>();
    if (!PoolSubsystem) return;
    // 归还 Actor 到池
    PoolSubsystem->ReleaseToPool(OwnerActor);
}

void UPoolableComponent::StartAutoReturnTimer()
{
	// 如果不需要自动回收
	if (AutoReturnTime <= 0.0f) return;
	// 获取世界
	const UWorld* World = GetWorld(); 
	if (!World) return;
	World->GetTimerManager().SetTimer(
		AutoReturnTimerHandle, // 保存句柄
		this, // 回调对象是自己
		&UPoolableComponent::ReturnToPool, // 计时器结束时调用 ReturnToPool
		AutoReturnTime, // 延迟秒数
		false // 不循环（只触发一次）
	);
}

void UPoolableComponent::ClearAutoReturnTimer()
{
	// 获取世界
    const UWorld* World = GetWorld();
    if (!World) return;
	// 清理定时器
    World->GetTimerManager().ClearTimer(AutoReturnTimerHandle);
}

void UPoolableComponent::ApplyActivateStateToActor(const FPoolSpawnInfo& InSpawnInfo, const FPoolSpawnOptions& InOptions)
{
	// 获取 Owner
    AActor* OwnerActor = GetOwner();
    if (!IsValid(OwnerActor)) return;
    // 设置 Owner 与自定义状态（避免串台）
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

	// 临时数组：用于存储组件，使用 InlineArray 避免每次都在堆上分配内存，提升高频调用的性能
	TInlineComponentArray<UActorComponent*, 12> Components(OwnerActor);
	// 获取所有组件
    OwnerActor->GetComponents(Components);
	// 遍历所有组件
    for (UActorComponent* Comp : Components)
    {
        // 如果组件无效，则跳过，继续遍历下一个
        if (!Comp) continue;
    	// 设置组件 Tick
        Comp->SetComponentTickEnabled(InOptions.bEnableComponentTick);
    	
        // 如果是投射物移动组件，则进行如下操作：
        if (UProjectileMovementComponent* ProjectileMoveComp = Cast<UProjectileMovementComponent>(Comp))
        {
        	/* 
        	 * 确保 UpdatedComponent 正确（池化时强烈建议每次都设一次），
        	 * 含义为告诉 UProjectileMovementComponent“我到底要推动哪个组件移动”。
        	 * 只有 UPrimitiveComponent 才具备：碰撞（Collision）物理（Physics）能参与 Sweep 移动，
        	 * Cast 成功：说明 Root 是 Sphere/Capsule/Mesh 这类 Primitive，可用作 UpdatedComponent，
        	 * Cast 失败：说明 Root 只是 SceneComponent（没碰撞），不适合给 ProjectileMovement 用。
        	 */
        	if (UPrimitiveComponent* RootPrimComp = Cast<UPrimitiveComponent>(OwnerActor->GetRootComponent()))
        	{ProjectileMoveComp->SetUpdatedComponent(RootPrimComp);}
        	// 清理上一轮残留（可留可不留，但留着更稳）
        	ProjectileMoveComp->StopMovementImmediately();
        	// 重新给速度（方向来自 InTransform 的旋转），GetForwardVector 表示当前朝向的前方单位向量
        	const FVector ForwardDir = InSpawnInfo.Transform.GetRotation().GetForwardVector();
        	// 设置一个“本次要用的速度值”——优先用 InitialSpeed，
        	float Speed = 0.0f;
        	if (ProjectileMoveComp->InitialSpeed > 0.0f)
        	{Speed = ProjectileMoveComp->InitialSpeed;}
        	// 如果<=0，就用 AScProjectileActor 中的默认速度兜底。
	        else
	        {
		        if (const AScProjectileActor* Actor = Cast<AScProjectileActor>(GetOwner()))
		        {Speed = Actor->DefaultInitialSpeed;}
	        }
        	// 设置速度向量
        	ProjectileMoveComp->Velocity = ForwardDir * Speed;
        	// 激活投射物移动组件
        	ProjectileMoveComp->Activate(true);
        }
        
        // 如果是 Primitive 碰撞体，处理碰撞预设。
        if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Comp))
        {
        	// 显示碰撞组件。
            PrimitiveComp->SetVisibility(true, true);
        	// 如果要求开碰撞，则开启查询与物理（可按需改）
            if (InOptions.bEnableCollision) 
            {PrimitiveComp->SetCollisionEnabled(InSpawnInfo.CompCollisionEnabledType);}
        	// 如果要清速度
            if (InOptions.bResetPhysicsVelocity) 
            {
            	// 清线速度
                PrimitiveComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
            	// 清角速度
                PrimitiveComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
            	// 如果物体是物理模拟的，可能需要唤醒它，否则它可能悬空静止
            	if (PrimitiveComp->IsSimulatingPhysics())
            	{PrimitiveComp->WakeAllRigidBodies();}
            }
        }
    	
    	// 如果要求激活特效
        if (InOptions.bActivateFXComponents)
        {
            // 如果是粒子特效，则激活粒子
            if (UParticleSystemComponent* ParticleSysComp = Cast<UParticleSystemComponent>(Comp)) 
            {ParticleSysComp->ActivateSystem(true);}
            // 如果是 Niagara，则激活 Niagara
            if (UNiagaraComponent* NiagaraComp = Cast<UNiagaraComponent>(Comp))
            {NiagaraComp->Activate(true);}
        }
    	// 如果要求激活音频
        if (InOptions.bActivateAudioComponents)
        {
            // 如果是音频，则播放音效
            if (UAudioComponent* AudioComp = Cast<UAudioComponent>(Comp))
            {AudioComp->Play();}
        }
    }
	// 最后启动自动回收（如果设置了 AutoReturnTime）
    StartAutoReturnTimer();
}

void UPoolableComponent::ApplyDeactivateStateToActor()
{
    // 获取 Owner
    AActor* OwnerActor = GetOwner();
    if (!IsValid(OwnerActor)) return;
    // 在游戏中隐藏 Actor（不渲染）
    OwnerActor->SetActorHiddenInGame(true);
    // Actor 层关碰撞，关闭 Actor 碰撞
    OwnerActor->SetActorEnableCollision(false);
    // 关闭 Actor Tick（省性能）
    OwnerActor->SetActorTickEnabled(false);
    // 清理归属者（Owner）与自定义状态（避免串台）
    OwnerActor->SetOwner(nullptr);
    OwnerActor->SetInstigator(nullptr);
    // 清理 Timer（如果有任何 Timer，一定要清）
    if (UWorld* World = GetWorld())
    {
    	// ClearAllTimersForObject 函数可清掉属于该对象的所有定时器（简单粗暴）
	    World->GetTimerManager().ClearAllTimersForObject(OwnerActor);
    	/* 
    	 * 清理所有蓝图潜伏动作（Latent Actions），如 Delay、Retriggerable Delay 节点，
    	 * 如果不加这一行，蓝图里的 Delay 可能会在 Actor 已经在池子里时到期执行，导致严重的逻辑 Bug
    	 */
    	World->GetLatentActionManager().RemoveActionsForObject(OwnerActor);
    }
	
	// 临时数组：用于存储组件，使用 InlineArray 避免每次都在堆上分配内存，提升高频调用的性能
	TInlineComponentArray<UActorComponent*, 12> Components(OwnerActor);
	// 获取所有组件
    OwnerActor->GetComponents(Components);
	// 遍历每个组件
    for (UActorComponent* Comp : Components)
    {
        // 组件无效则跳过，继续检查下一个。
        if (!Comp) continue;
        // 关闭组件 Tick
        Comp->SetComponentTickEnabled(false);
    	
        // 如果是投射物移动组件，则进行如下操作：
        if (UProjectileMovementComponent* ProjectileMoveComp = Cast<UProjectileMovementComponent>(Comp))
        {
            // 停运动
            ProjectileMoveComp->StopMovementImmediately();
        	// 关闭组件（不再 Tick）
            ProjectileMoveComp->Deactivate();
        }
    	
    	// 如果是 Primitive （碰撞/物理）组件，则进行如下操作：
    	if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Comp))
    	{
    		PrimitiveComp->SetSimulatePhysics(false); // 关闭物理模拟（可避免回池后还在飞）
    		PrimitiveComp->SetPhysicsLinearVelocity(FVector::ZeroVector); // 清线速度
    		PrimitiveComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector); // 清角速度
    		PrimitiveComp->SetVisibility(false, true); // 组件也隐藏（递归子组件）
    		PrimitiveComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 组件碰撞关闭
    	}

        // 如果是普通粒子组件，则停止粒子播放
        if (UParticleSystemComponent* ParticleSysComp = Cast<UParticleSystemComponent>(Comp)) 
        {ParticleSysComp->DeactivateSystem();}

         // 如果是 Niagara 组件，则停止 Niagara
        if (UNiagaraComponent* NiagaraComp = Cast<UNiagaraComponent>(Comp)) 
        {NiagaraComp->Deactivate();}

         // 如果是音频组件，则停止声音
        if (UAudioComponent* AudioComp = Cast<UAudioComponent>(Comp))
        {AudioComp->Stop();}        
    }
}