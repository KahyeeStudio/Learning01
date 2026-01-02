// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ScGameplayAbilityBase.h"
#include "Navigation/PathFollowingComponent.h"
#include "ScEnemySearchForTarget.generated.h"

/**
 * 敌人索敌技能的C++类，可替代蓝图类。
 */

namespace EPathFollowingResult
{
	enum Type : int;
}

class AScEnemyCharacter;
class AAIController;
class UScWaitGameplayEvent;
class UAbilityTask_WaitDelay;
class AScCharacterBase;
class UAITask_MoveTo;

UCLASS()
class PROJECTSCAVENGER_API UScEnemySearchForTarget : public UScGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	
	UScEnemySearchForTarget();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	TWeakObjectPtr<AScEnemyCharacter> OwningEnemy;
	TWeakObjectPtr<AAIController> OwningAIController;
	TWeakObjectPtr<AScCharacterBase> TargetBaseCharacter;
	
private:
	
	UPROPERTY()
	TObjectPtr<UScWaitGameplayEvent> WaitGameplayEventTask;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> SearchDelayTask;
	
	UPROPERTY()
	TObjectPtr<UAITask_MoveTo> MoveToLocationOrActorTask;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> AttackDelayTask;
	
	void StartSearch();
	
	UFUNCTION()
	void EndAttackEventReceived(FGameplayEventData Payload);
	
	// SearchDelayTask的回调函数。
	UFUNCTION()
	void SearchOn();
	
	void MoveToTargetAndAttack();
	
	UFUNCTION()
	void AttackTarget(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController);
	
	// AttackDelayTask的回调函数。
	UFUNCTION()
	void AttackOn();
	
};
