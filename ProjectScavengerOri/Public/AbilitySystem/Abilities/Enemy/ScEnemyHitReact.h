// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ScGameplayAbilityBase.h"
#include "ScEnemyHitReact.generated.h"

/**
 *
 */

UCLASS()
class PROJECTSCAVENGER_API UScEnemyHitReact : public UScGameplayAbilityBase
{
	GENERATED_BODY()

public:

	/** 缓存AvatarForward和ToInstigator两个向量。
	  * @param Instigator 攻击发起者的AActor。 
	  */
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Abilities")
	void CacheHitDirectionVectors(AActor* Instigator);

	/** 拥有此技能的Avatar的前方向量，需调用CacheHitDirectionVectors函数后才有效。*/
	UPROPERTY(BlueprintReadOnly, Category = "Scavenger|Abilities")
	FVector AvatarForward;

	/** 拥有此技能的Avatar位置至攻击发起者位置的向量，需调用CacheHitDirectionVectors函数后才有效。*/
	UPROPERTY(BlueprintReadOnly, Category = "Scavenger|Abilities")
	FVector ToInstigator;

};