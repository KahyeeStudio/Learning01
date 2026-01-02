// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "ScWaitGameplayEvent.generated.h"

/**
 * 用于游戏技能的异步任务，可通过该子类调用AbilityAsync_WaitGameplayEvent.h中的函数。
 */

UCLASS()
class PROJECTSCAVENGER_API UScWaitGameplayEvent : public UAbilityAsync_WaitGameplayEvent
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Ability|Async", meta = (DefaultToSelf = "TargetActor", BlueprintInternalUseOnly = "TRUE"))
	static UScWaitGameplayEvent* WaitGameplayEventToActorProxy(AActor* TargetActor, UPARAM(meta = (GameplayTagFilter="GameplayEventTagsCategory")) FGameplayTag EventTag, bool OnlyTriggerOnce = false, bool OnlyMatchExact = true);
	
	void StartActivation();
	
};
