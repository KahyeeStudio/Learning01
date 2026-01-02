// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AttributeSet.h"
#include "ScAttributeChangeTask.generated.h"

class UAbilitySystemComponent;//Attribute.h中已有此前项声明，为保代码健壮性，此处再手动添加一次。
struct FOnAttributeChangeData;

/**
 * 异步任务节点，负责监听属性变化。
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);

UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class PROJECTSCAVENGER_API UScAttributeChangeTask : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnAttributeChanged;

	/** 异步任务节点，负责监听属性变化。
	 * OnAttributeChanged引脚将在内部委托被广播时触发。
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UScAttributeChangeTask* ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute);
	
	// 用于清理任务，释放资源。
	UFUNCTION(BlueprintCallable)
	void EndTask();
	
	TWeakObjectPtr<UAbilitySystemComponent> ASC;
	FGameplayAttribute AttributeToListenFor;
	
	void AttributeChanged(const FOnAttributeChangeData& Data);
};
