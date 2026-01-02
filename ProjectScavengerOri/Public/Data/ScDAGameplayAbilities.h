// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTags.h"
#include "ScDAGameplayAbilities.generated.h"

/**
 * 游戏技能的数据资产C++类，需在UE内容浏览器中创建数据资产实例。
 */

class UGameplayAbility;
class UGameplayEffect;
class UAnimMontage;
class UNiagaraSystem;
class USoundBase;

UCLASS(Blueprintable)
class PROJECTSCAVENGER_API UScDAGameplayAbilities : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	/** 游戏技能类(GA)。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> AbilityClass;
		
	/** 伤害效果类(GE)。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	/** 伤害值的标签(Set By Caller Magnitude Data Tag)。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag DamageDataTag;	
	
	/** 伤害值(Set By Caller Magnitude)。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageValue = 10.0f;
	
	/** 冷却效果类(GE)。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> CooldownEffectClass;
	
	/** 冷却时间的标签(Set By Caller Magnitude Data Tag)。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag CooldownDataTag;
	
	/** 冷却时间的值(Set By Caller Magnitude)。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CooldownValue = 5.0f;
	
	/** 消耗效果类(GE)。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> ManaCostEffectClass;
	
	/** 消耗资源的标签(Set By Caller Magnitude Data Tag)。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ManaCostDataTag;
	
	/** 消耗资源的值(Set By Caller Magnitude)。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ManaCostValue = 5.0f;
	
	/** 发送的游戏事件标签。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag EventTag;
	
	/** 攻击动画。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> AttackMontage;
	
	/** 攻击动画数组。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UAnimMontage>> AttackMontages;
	
	/** 攻击粒子特效。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UNiagaraSystem> AttackParticle;
	
	/** 旧版攻击粒子特效。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParticleSystem> AttackCascade;
	
	/** 攻击音效。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> AttackSFX;
	
	/** 受击动画。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	/** 受击动画数组。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UAnimMontage>> HitReactMontages;
	
	/** 受击粒子特效。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UNiagaraSystem> HitReactParticle;
	
	/** 旧版受击粒子特效。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParticleSystem> HitReactCascade;
	
	/** 受击音效。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> HitReactSFX;
	
};