// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
// 从自定义的属性集中获取已有属性，必须添加头文件，不可使用前项声明。
#include "AbilitySystem/ScAttributeSet.h"
#include "ScAttributeWidget.generated.h"

/**
 * 用于显示属性集中的属性值，可处理所有属性，可复用的UI控件。
 */

UCLASS()
class PROJECTSCAVENGER_API UScAttributeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scavenger|Attributes")
	FGameplayAttribute Attribute;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scavenger|Attributes")
	FGameplayAttribute MaxAttribute;
	
	/**
	 * 用于处理属性值变化。
	 */
	void OnAttributeChange(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair, UScAttributeSet* AttributeSet, float OldValue);
	
	/**
	 * Pair.Key是Attribute，Pair.Value是MaxAttribute。 
	 */
	bool MatchesAttributes(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const;
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "On Attribute Change"))
	void BP_OnAttributeChange(float OldValue, float NewValue, float NewMaxValue);
	
protected:
	
	// 是否允许折叠可见性。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scavenger")
	bool bAllowedCollapesd = false;
	
};
