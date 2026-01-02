// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "AbilitySystem/Abilities/ScProjectileAbility.h"
//#include "Kismet/KismetSystemLibrary.h"
#include "GameObjects/ScProjectileActor.h"
#include "Interaction/CombatInterface.h"
#include "Managers/PoolSubsystem.h"


void UScProjectileAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!GetAbilitySystemComponentFromActorInfo()) return;
}

void UScProjectileAbility::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	/* 
	 * 父类的 ActivateAbility 函数内部逻辑是“同步”调用蓝图事件的。
	 * 简单来说，当你在 C++ 中调用 Super::ActivateAbility(...) 时，
	 * 由于 GAS 的底层机制，它会立即触发并运行完整个蓝图中的 ActivateAbility 事件，
	 * 然后才会返回到你的 C++ 代码中继续执行剩下的语句（即你的 C++ 打印）。
	 * 详细执行流程拆解：
	 * 让我们一步步跟踪代码的执行顺序，看看为什么日志是先输出“Blueprint”后输出“C++”的：
	 * C++ 函数被调用 当你激活技能时，系统首先调用 C++ 的入口函数： UScProjectileAbility::ActivateAbility(C++)
	 * 进入 Super 也就是父类逻辑 代码执行到第一行： Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	 * 这一步非常关键。在 UGameplayAbility（父类）的内部实现中，它会检查这个技能是否有蓝图版本。
	 * 如果检测到有蓝图实现，它会立即调用蓝图中的事件 K2_ActivateAbility（对应你在蓝图里看到的 Event ActivateAbility）。
	 * 蓝图逻辑执行（插入点） 此时，C++ 的执行流暂停在 Super 这一行，转而进入蓝图虚拟机：
	 * 执行蓝图节点：Parent: ActivateAbility（这是蓝图层面的父类调用，通常对于 GAS 基础事件来说它是空的或者做一些基础初始化）。
	 * 执行蓝图节点：Print String。
	 * 日志输出： [GA_SmallFireBall_Player_C_0] GA_SmallFIreBall: ActivateAbility(Blueprint)
	 * 蓝图执行完毕，返回 C++ 蓝图逻辑跑完后，控制权返回给 C++ 的 Super::ActivateAbility，然后 Super 函数执行完毕并返回。
	 * C++ 继续执行剩余代码 现在，代码终于来到了你 C++ 函数的下一行： UKismetSystemLibrary::PrintString(...)
	 * 日志输出： [GA_SmallFireBall_Player_C_0] UScProjectileAbility: ActivateAbility(C++)
	 * 图解逻辑栈
	 * 为了更直观地理解，你可以将其想象为一个“三明治”结构：
	 * [开始] UScProjectileAbility::ActivateAbility (你的 C++ 代码开始)
	 * [调用] Super::ActivateAbility
	 * [内部触发] 蓝图 Event ActivateAbility
	 * 打印："Blueprint" (先发生)
	 * [结束] 蓝图运行结束
	 * [返回] Super 调用结束
	 * 打印："C++" (后发生)
	 * [结束] 函数结束
	 */
	
	// 这个库函数等同于蓝图中的 PrintString 函数。
	//UKismetSystemLibrary::PrintString(this, FString("UScProjectileAbility: ActivateAbility(C++)"), true, true, FLinearColor::Yellow, 3);
	
	/* 
	 * GAS中有一个内置的函数，可用于判断是否具有服务器权限，ActivationInfo 可直接调用。
	 * 如果不从 ActivationInfo 调用的话，参数需要一个 ActivationInfo 指针，
	 */
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ProjectileClass无效，请检查GA的默认设置。"));
		return;
	}
	// 使用CombatInterface接口中的函数可获取枪口位置。
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface) return;
	const FVector SocketLocation = CombatInterface->GetMuzzleSocketLocation();
	// 用鼠标指针的位置的向量减枪口位置的向量得到投射物的发射角度。
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	// 用鼠标指针的位置的向量减角色位置的向量得到投射物的发射角度。
	//FRotator Rotation = (ProjectileTargetLocation - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
	/*
	 * Pitch = 0 的意思是：把上下仰角强行清零，只保留水平面上的朝向（Yaw）。
	 * Pitch：绕 Y 轴 的旋转，通俗讲就是“抬头/低头”的角度（上下看）。
	 * Yaw：绕 Z 轴 的旋转，就是“左右转身/朝向”的角度。
	 * Roll：绕 X 轴 的旋转，就是“侧倾/翻滚”的角度。
	 */
	Rotation.Pitch = 0.0f;
	// 构造一个Transform，把枪口位置赋给它，用于在枪口位置生成投射物。
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());
	
	/*
	 * SpawnActorDeferred 是延迟生成Actor的函数。
	 * GAS有一个内置函数GetOwningActorFromActorInfo()可用于获取Owner。
	 * SpawnActorDeferred: 先创建实例，但不走“构造流程”：会执行 C++ 构造函数（AActor()），
	 * 但不会立刻执行 OnConstruction / 蓝图 Construction Script，也不会立刻走 BeginPlay。
	 * 你可以在这段“空窗期”里，把需要的参数/引用/组件状态先塞进去（例如：伤害数值、GE Spec、Instigator、忽略碰撞的Actor列表、初速度、
	 * 队伍ID、绑定子组件等）。
	 * 然后必须调用 FinishSpawning(...)（或 UGameplayStatics::FinishSpawningActor）来完成生成：
	 * 这一步才会触发 Construction / 组件初始化，并在后续正常进入 BeginPlay。
	 */
	
	// 构造对象池生成信息结构体。
	FPoolSpawnInfo SpawnInfo;
	SpawnInfo.Transform = SpawnTransform;
	SpawnInfo.Owner = GetOwningActorFromActorInfo();
	SpawnInfo.Instigator = Cast<APawn>(GetOwningActorFromActorInfo());
	SpawnInfo.CollisionHandlingMethodOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	// 构造对象池生成选项结构体。
	FPoolSpawnOptions SpawnOptions;
	// 获取对象池子系统并检查有效性。
	if (UPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UPoolSubsystem>())
	{
		
		// 从对象池中取出组件并获取取出的Actor。
		PoolSubsystem->AcquireFromPool(ProjectileClass, SpawnInfo, SpawnOptions);
		
	}
	
	// 这里可以给投射物添加GE，用于处理伤害。
	
}
