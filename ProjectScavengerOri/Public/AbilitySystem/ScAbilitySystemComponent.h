// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ScAbilitySystemComponent.generated.h"

/**
 *
 */

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTSCAVENGER_API UScAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRep_ActivateAbilities() override;

	/**
	 * 服务器函数，设置技能等级，并将技能规格标记为Diety以便下次网络更新时同步。
	 * @param AbilityClass 想要设置的技能类，需选择蓝图类。 
	 * @param Level 想要设置为的技能等级。
	 */
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Abilities")
	void SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level);
	
	/**
	 * 服务器函数，将技能等级加1，并将技能规格标记为Diety以便下次网络更新时同步。
	 * @param AbilityClass 想要设置的技能类，需选择蓝图类。 
	 * @param Level 默认为1。
	 */
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Abilities")
	void AddAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Value = 1);

protected:

	//virtual void BeginPlay() override;

private:

	void HandleAutoActivatedAbility(const FGameplayAbilitySpec& AbilitySpec);
};