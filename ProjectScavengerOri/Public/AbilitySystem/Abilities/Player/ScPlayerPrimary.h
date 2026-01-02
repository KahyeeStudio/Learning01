// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ScGameplayAbilityBase.h"
#include "ScPlayerPrimary.generated.h"

/**
 * 主要技能，Primary Ability，普通攻击。
 */

UCLASS()
class PROJECTSCAVENGER_API UScPlayerPrimary : public UScGameplayAbilityBase
{
	GENERATED_BODY()

public:

	/** 重叠检测后，发送击中事件至被检测到的Actor。*/
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Abilities")
	void SendHitReactEventToActors(const TArray<AActor*> ActorsHit);

protected:

	/** 碰撞盒体半径。*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scavenger|Abilities")
	float HitBoxRadius = 100.0f;

	/** 碰撞盒体向前偏移量。*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scavenger|Abilities")
	float HitBoxForwardOffset = 200.0f;

	/** 碰撞盒体向上偏移量。*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scavenger|Abilities")
	float HitBoxElevationOffset = 20.0f;

};