// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "UI/ScAttributeWidget.h"

void UScAttributeWidget::OnAttributeChange(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair,	UScAttributeSet* AttributeSet, float OldValue)
{
	// 在蓝图中初始化时，必须先初始化最大值，后初始化当前值。
	const float MaxAttributeValue = Pair.Value.GetNumericValue(AttributeSet);
	const float AttributeValue = Pair.Key.GetNumericValue(AttributeSet);
	BP_OnAttributeChange(OldValue, AttributeValue, MaxAttributeValue);
}

bool UScAttributeWidget::MatchesAttributes(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const
{
	return Attribute == Pair.Key && MaxAttribute == Pair.Value;
}
