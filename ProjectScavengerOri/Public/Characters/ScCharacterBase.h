// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "ScCharacterBase.generated.h"

namespace ScTags
{
	extern PROJECTSCAVENGER_API const FName Player;
}

class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
struct FOnAttributeChangeData;

// 声明一个委托，用于广播AbilitySystemComponent初始化。
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FASCInitialized, UAbilitySystemComponent*, ASC, UAttributeSet*, AS);

UCLASS(Abstract)
class PROJECTSCAVENGER_API AScCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AScCharacterBase();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual UAttributeSet* GetAttributeSet() const { return nullptr; }

	UPROPERTY(BlueprintAssignable)
	FASCInitialized OnASCInitialized;
	
	bool IsAlive() const { return bAlive; }
	void SetAlive(bool bInAliveStatus) { bAlive = bInAliveStatus; }
	
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Death")
	virtual void HandleRespawn();
	
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Attributes")
	void ResetAttributes();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Scavenger|Abilities")
	void RotateToTarget(AActor* RotateTarget);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Scavenger|Abilities")
	bool bIsBeingLaunched = false;
	
	UFUNCTION(Blueprintable, Category = "Scavenger|Abilities")
	virtual void StopMovementUntilLanded();
	
	/** 搜索范围，大于0时应用指定范围，小于等于0时应用全场范围，默认值为2000.0f。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scavenger|Abilities")
	float SearchRange = 2000.0f;
	
protected:

	void GiveStartupAbilities();

	void InitializeAttributes() const;
	
	// 监听生命值变化的回调函数。
	void OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData);
	// 处理死亡事件。
	virtual void HandleDeath();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Scavenger|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Scavenger|Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Scavenger|Effects")
	TSubclassOf<UGameplayEffect> ResetAttributesEffect;
	
	// 是否存活，这是一个可复制的变量。
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Replicated)
	bool bAlive = true;
};