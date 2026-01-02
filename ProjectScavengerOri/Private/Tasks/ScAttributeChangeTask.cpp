// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "Tasks/ScAttributeChangeTask.h"
#include "AbilitySystemComponent.h"

UScAttributeChangeTask* UScAttributeChangeTask::ListenForAttributeChange(
	UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute)
{
	// 当函数被调用时，创建一个实例。
	UScAttributeChangeTask* WaitForAttributeChangeTask = NewObject<UScAttributeChangeTask>();
	// 初始化。
	WaitForAttributeChangeTask->ASC = AbilitySystemComponent;
	WaitForAttributeChangeTask->AttributeToListenFor = Attribute;
	// 检查技能系统组件有效性。
	if (IsValid(AbilitySystemComponent))
	{
		// 有效时绑定属性变更的回调函数。
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(WaitForAttributeChangeTask, &UScAttributeChangeTask::AttributeChanged);
		return WaitForAttributeChangeTask;
	}
	else
	{
		// 无效时需要清理上面设置的根集合。
		WaitForAttributeChangeTask->RemoveFromRoot();
		return nullptr;
	}
}

void UScAttributeChangeTask::EndTask()
{
	if (ASC.IsValid())
	{
		// 解绑委托。
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this);		
	}
	SetReadyToDestroy();
	MarkAsGarbage();
}

void UScAttributeChangeTask::AttributeChanged(const FOnAttributeChangeData& Data)
{
	// 广播创建的委托，这将触发蓝图中的对应引脚。
	OnAttributeChanged.Broadcast(Data.Attribute, Data.NewValue, Data.OldValue);
}
