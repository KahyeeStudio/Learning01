// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ScGameplayAbility.h"
#include "ScProjectileAbility.generated.h"

/**
 * 投射物类型的GameplayAbility的C++基类。
 */

class AScProjectileActor;

UCLASS()
class A1PROJECTSCAVENGER_API UScProjectileAbility : public UScGameplayAbility
{
	GENERATED_BODY()
	
protected:
	
	/** 在C++类中重写的激活技能函数。*/
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	/** 用于生成投射物的C++函数。*/
	UFUNCTION(BlueprintCallable, Category="Scavenger|Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation);
	
	/** 在蓝图中指定需要生成的ScProjectile子类。*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AScProjectileActor> ProjectileClass;
};