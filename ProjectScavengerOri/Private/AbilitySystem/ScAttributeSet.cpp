// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "AbilitySystem/ScAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/ScGameplayTags.h"

void UScAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// REPNOTIFY_Always表示即使值没有变化也通知。
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxMana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME(ThisClass, bAttributesInitialized);
}

void UScAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// 回调数据中有一个评估数据，判断应用的GE影响的是否为生命值属性，且当前属性的值小于0，则死亡。
	if (Data.EvaluatedData.Attribute == GetHealthAttribute() && GetHealth() <= 0.0f)
	{
		// 制作一个Payload。
		FGameplayEventData Payload;
		// 将Payload中的发起者设置为此GE回调数据中的目标的Avatar。
		Payload.Instigator = Data.Target.GetAvatarActor();
		// 向此GE的发起者发送一个游戏事件，让它知道它击杀了某个Character。
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Data.EffectSpec.GetEffectContext().GetInstigator(), ScGameplayTags::Events::KillScored, Payload);
	}
	
	// 如果属性未被初始化，则在此初始化属性。
	if (!bAttributesInitialized)
	{
		// 设置已初始化变量为真。
		bAttributesInitialized = true;
		// 广播。
		OnAttributesInitialized.Broadcast();
	}
}

void UScAttributeSet::OnRep_AttributesInitialized()
{
	if (bAttributesInitialized)
	{
		OnAttributesInitialized.Broadcast();
	}
}

void UScAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldValue);
}

void UScAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldValue);
}

void UScAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Mana, OldValue);
}

void UScAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxMana, OldValue);
}
