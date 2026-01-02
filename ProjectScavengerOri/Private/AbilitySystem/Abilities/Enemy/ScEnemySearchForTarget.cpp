// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "AbilitySystem/Abilities/Enemy/ScEnemySearchForTarget.h"
#include "AbilitySystemComponent.h"
#include "Characters/ScEnemyCharacter.h"
#include "AIController.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "AbilitySystem/AbilityTasks/ScWaitGameplayEvent.h"
#include "GameplayTags/ScGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Libraries/ScGASFunctionLibrary.h"
#include "Tasks/AITask_MoveTo.h"

UScEnemySearchForTarget::UScEnemySearchForTarget()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
}

void UScEnemySearchForTarget::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	// 缓存弱指针。
	OwningEnemy = Cast<AScEnemyCharacter>(GetAvatarActorFromActorInfo());
	check(OwningEnemy.IsValid());
	OwningAIController = Cast<AAIController>(OwningEnemy->GetController());
	check(OwningAIController.IsValid());
	
	StartSearch();
	// C++中处理WaitGameplayEvent的方式是将实现函数绑定到WaitGameplayEvent内部的委托上，然后激活。
	WaitGameplayEventTask = UScWaitGameplayEvent::WaitGameplayEventToActorProxy(GetAvatarActorFromActorInfo(), ScGameplayTags::Events::Enemy::EndAttack);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::EndAttackEventReceived);
	WaitGameplayEventTask->StartActivation();
}

void UScEnemySearchForTarget::StartSearch()
{
	if (bDrawDebugs) GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("C++ Start Search.")));
	if (!OwningEnemy.IsValid()) return;
	const float SearchDelay = FMath::RandRange(OwningEnemy->MinAttackDelay, OwningEnemy->MaxAttackDelay);
	SearchDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, SearchDelay);
	SearchDelayTask->OnFinish.AddDynamic(this, &ThisClass::SearchOn);
	SearchDelayTask->Activate();	
}

void UScEnemySearchForTarget::EndAttackEventReceived(FGameplayEventData Payload)
{
	if (OwningEnemy.IsValid() && !OwningEnemy->bIsBeingLaunched)
	{
		StartSearch();
	}
}

void UScEnemySearchForTarget::SearchOn()
{
	const FVector SearchOrigin = GetAvatarActorFromActorInfo()->GetActorLocation();
	FClosestActorWithTagResult ClosestActorResult = UScGASFunctionLibrary::FindClosestActorWithTag(GetAvatarActorFromActorInfo(), SearchOrigin, ScTags::Player);
	TargetBaseCharacter = Cast<AScCharacterBase>(ClosestActorResult.Actor);
	if (!TargetBaseCharacter.IsValid())
	{
		StartSearch();
		return;
	}
	if (TargetBaseCharacter->IsAlive())
	{
		MoveToTargetAndAttack();
	}
	else
	{
		StartSearch();
	}
}

void UScEnemySearchForTarget::MoveToTargetAndAttack()
{
	if (!OwningEnemy.IsValid() || !OwningAIController.IsValid() || !TargetBaseCharacter.IsValid()) return;
	if (!OwningEnemy->IsAlive())
	{
		StartSearch();
		return;
	}
	MoveToLocationOrActorTask = UAITask_MoveTo::AIMoveTo(OwningAIController.Get(), FVector(), TargetBaseCharacter.Get(), OwningEnemy->AcceptanceRadius);
	MoveToLocationOrActorTask->OnMoveTaskFinished.AddUObject(this, &ThisClass::AttackTarget);
	MoveToLocationOrActorTask->ConditionalPerformMove();
}

void UScEnemySearchForTarget::AttackTarget(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController)
{
	// 如果敌人因某种原因失败，返回并重新搜索。
	if (Result != EPathFollowingResult::Success)
	{
		StartSearch();
		return;		
	}
	OwningEnemy->RotateToTarget(TargetBaseCharacter.Get());
	// 设置延迟等待敌人转身完成。
	AttackDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, OwningEnemy->GetTimelineLength());
	AttackDelayTask->OnFinish.AddDynamic(this, &ThisClass::AttackOn);
	AttackDelayTask->Activate();
}

void UScEnemySearchForTarget::AttackOn()
{
	const FGameplayTag AttackTag = ScGameplayTags::Abilities::Enemy::Attack;
	GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(AttackTag.GetSingleTagContainer());
}
