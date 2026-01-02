// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ScGameplayAbilityBase.generated.h"

/**
 * 游戏技能的基类。
 */

class UScDAGameplayAbilities;

UCLASS()
class PROJECTSCAVENGER_API UScGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scavenger")
	TObjectPtr<UScDAGameplayAbilities> AbilityConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scavenger|Debug")
	bool bDrawDebugs = false;
};