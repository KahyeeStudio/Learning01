// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "Managers/PoolSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Managers/PoolableComponent.h"
//#include "Kismet/GameplayStatics.h" // 可选：如果你后面想要更方便获取世界信息


void UPoolSubsystem::Deinitialize()
{
    Super::Deinitialize();
    // 遍历所有池
    for (TPair<TObjectPtr<UClass>, FScActorPool>& Pair : Pools)
    {
        // 获取池数据
        FScActorPool& Pool = Pair.Value;
        // 遍历闲置 Actor
        for (TWeakObjectPtr<AActor>& WeakActor : Pool.InactiveActors)
        {
            // 尝试取出池内闲置的 Actor 的指针，如果还有效，则在世界销毁时直接 Destroy（安全）
            if (AActor* Actor = WeakActor.Get(); IsValid(Actor))
            {Actor->Destroy();}
        }
        // 清空数组        
        Pool.InactiveActors.Empty();
        // 清统计
        Pool.TotalCreated = 0;
    }
    // 清空 Map
    Pools.Empty();
}

void UPoolSubsystem::Prewarm(TSubclassOf<AActor> ActorClass, int32 Count)
{
    // 类无效，返回
    if (!ActorClass) return;
    // 数量不合法，返回
    if (Count <= 0) return;
    
    FPoolSpawnOptions Options; // 创建默认 Options
    Options.bUnhideActor = false; // 预热后我们会归还到池，所以先不显示
    Options.bEnableActorTick = false; // 预热后不 Tick
    Options.bEnableCollision = false; // 预热后不开碰撞

    for (int32 i = 0; i < Count; ++i) // 循环 Count 次
    {
        // 创建 FPoolSpawnInfo 结构体。
        FPoolSpawnInfo SpawnInfo;
        // 先 Acquire（会 Spawn）
        AActor* Actor = AcquireFromPool(ActorClass, SpawnInfo, Options);
        // 如果成功，先立刻归还到池。
        if (IsValid(Actor))
        {ReleaseToPool(Actor);}
    }
}

AActor* UPoolSubsystem::AcquireFromPool(const TSubclassOf<AActor> ActorClass, const FPoolSpawnInfo& SpawnInfo, const FPoolSpawnOptions& Options)
{
    // 如果类无效，返回空
    if (!ActorClass) return nullptr;
    // 获取 UClass 指针
    UClass* ClassKey = ActorClass.Get();
    // 如果无效，返回空
    if (!ClassKey) return nullptr;
    // 找到或创建该类的池
    FScActorPool& Pool = Pools.FindOrAdd(ClassKey);
    // 只要池里还有闲置 Actor
    while (Pool.InactiveActors.Num() > 0)
    {
        // 从末尾弹一个（O(1)）
        TWeakObjectPtr<AActor> WeakActor = Pool.InactiveActors.Pop(EAllowShrinking::No);
        // 转成强指针（临时）
        AActor* Actor = WeakActor.Get();
        // 如果无效（可能关卡切换、GC、Destroy），继续拿下一个
        if (!IsValid(Actor)) continue;    
        // 找池化组件
        UPoolableComponent* Poolable = FindPoolableComponent(Actor);
        
        // TODO: 把取出和激活拆开，避免预热时自动激活。
        
        // 如果有池化组件
        if (Poolable) 
        {
            // 用组件方式激活（最完整）
            Poolable->ActivatePoolActor(SpawnInfo, Options);
        }
        // 如果没有组件
        else
        {
            if (Options.bSetTransform) // 需要设置 Transform
            {Actor->SetActorTransform(SpawnInfo.Transform);}
            Actor->SetActorHiddenInGame(!Options.bUnhideActor); // 显示/隐藏
            Actor->SetActorTickEnabled(Options.bEnableActorTick); // Tick
            Actor->SetActorEnableCollision(Options.bEnableCollision); // 碰撞
        }
        // 返回从池中复用出来的 Actor，可以直接被引用。
        return Actor; 
    }
    // 没有可复用就新建
    AActor* NewActor = SpawnNewActor(ActorClass, SpawnInfo); 
    // 如果无效，返回空
    if (!IsValid(NewActor)) return nullptr;
    Pool.TotalCreated += 1; // 统计 +1
    // 找池化组件
    UPoolableComponent* Poolable = FindPoolableComponent(NewActor);
    // 如果有池化组件则从池中激活（并可能启动自动回收）
    if (Poolable)
    {Poolable->ActivatePoolActor(SpawnInfo, Options);}
    // 没有组件也能用，但建议给可池化对象都加组件
    else
    {
        if (Options.bSetTransform)
        {NewActor->SetActorTransform(SpawnInfo.Transform);}
        NewActor->SetActorHiddenInGame(!Options.bUnhideActor);
        NewActor->SetActorTickEnabled(Options.bEnableActorTick);
        NewActor->SetActorEnableCollision(Options.bEnableCollision);
    }
    // 返回新 Actor
    return NewActor;
}

void UPoolSubsystem::ReleaseToPool(AActor* Actor)
{
    // 如果 Actor 无效，返回
    if (!IsValid(Actor)) return; 
    // 获取 Actor 的实际类
    UClass* ClassKey = Actor->GetClass();
    // 检查有效性。
    if (!ClassKey) return;
    // 获取对应池（没有就创建）
    FScActorPool& Pool = Pools.FindOrAdd(ClassKey);
    // 找池化组件
    UPoolableComponent* Poolable = FindPoolableComponent(Actor);
    
    // TODO: 把归还和休眠拆开。
    
    // 如果有组件，让 Actor 进入休眠态（隐藏/关碰撞/停特效等）
    if (Poolable)
    {Poolable->DeactivatePoolActor();}
    // 如果没有组件
    else
    {
        Actor->SetActorHiddenInGame(true); // 最简单的休眠：隐藏
        Actor->SetActorEnableCollision(false); // 关碰撞
        Actor->SetActorTickEnabled(false); // 关 Tick
    }
    // 放回闲置数组（弱引用）
    Pool.InactiveActors.Add(Actor);
}

AActor* UPoolSubsystem::SpawnNewActor(const TSubclassOf<AActor> ActorClass, const FPoolSpawnInfo& SpawnInfo)
{
    // 获取世界
    UWorld* World = GetWorld();
    if (!World) return nullptr;
    // 构造一个生成参数
    FActorSpawnParameters Params;
    // 填入结构体中的信息
    Params.Owner = SpawnInfo.Owner.Get();
    Params.Instigator = SpawnInfo.Instigator.Get();    
    Params.SpawnCollisionHandlingOverride = SpawnInfo.CollisionHandlingMethodOverride;// 默认强制生成（池化一般不考虑生成失败）
    Params.TransformScaleMethod = SpawnInfo.TransformScaleMethodOverride;
    Params.bDeferConstruction = false; // 不延迟构造（新手先别搞 deferred）
    // 生成 Actor
    AActor* NewActor = World->SpawnActor<AActor>(ActorClass, SpawnInfo.Transform, Params);
    return NewActor;
}

UPoolableComponent* UPoolSubsystem::FindPoolableComponent(AActor* Actor) const
{
    // 找不到对象池组件则返回空指针。
    if (!IsValid(Actor)) return nullptr;
    // 查找并返回组件。
    return Actor->FindComponentByClass<UPoolableComponent>();
}