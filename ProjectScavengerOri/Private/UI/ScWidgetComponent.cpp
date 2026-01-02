// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "UI/ScWidgetComponent.h"
#include "Characters/ScCharacterBase.h"
#include "AbilitySystem/ScAbilitySystemComponent.h"
#include "AbilitySystem/ScAttributeSet.h"
#include "UI/ScAttributeWidget.h"
#include "Blueprint/WidgetTree.h"

void UScWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	InitAbilitySystemData();

	// 如果AbilitySystemComponent未初始化，则绑定一个在初始化时广播的委托。
	if (!IsASCInitialized())
	{
		ScCharacter->OnASCInitialized.AddDynamic(this, &ThisClass::OnASCInitialized);
	}
	else
	{
		InitializeAttributeDelegate();
	}
}

void UScWidgetComponent::InitAbilitySystemData()
{
	ScCharacter = Cast<AScCharacterBase>(GetOwner());
	AttributeSet = Cast<UScAttributeSet>(ScCharacter->GetAttributeSet());
	AbilitySystemComponent = Cast<UScAbilitySystemComponent>(ScCharacter->GetAbilitySystemComponent());
}

bool UScWidgetComponent::IsASCInitialized() const
{
	return AbilitySystemComponent.IsValid() && AttributeSet.IsValid();
}

void UScWidgetComponent::InitializeAttributeDelegate()
{
	if (!AttributeSet->bAttributesInitialized)
	{
		AttributeSet->OnAttributesInitialized.AddDynamic(this, &ThisClass::BindToAttributeChanges);
	}
	else
	{
		BindToAttributeChanges();
	}
}

void UScWidgetComponent::OnASCInitialized(UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	AbilitySystemComponent = Cast<UScAbilitySystemComponent>(ASC);
	AttributeSet = Cast<UScAttributeSet>(AS);

	// 检查属性集是否已经随GameplayEffect初始化，如果没有，则被广播时绑定到一个委托上。
	if (IsASCInitialized())
	{
		InitializeAttributeDelegate();
	}
	else
	{
		return;
	}
}

void UScWidgetComponent::BindToAttributeChanges()
{
	for (const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair : AttributeMap)
	{
		// 检查已拥有的控件。
		BindWidgetToAttributeChanges(GetUserWidgetObject(), Pair);
		// 处理未拥有的控件，遍历所有子控件。
		GetUserWidgetObject()->WidgetTree->ForEachWidget([this, &Pair](UWidget* ChildWidget)
		{
			BindWidgetToAttributeChanges(ChildWidget, Pair);
		});
	}
}

void UScWidgetComponent::BindWidgetToAttributeChanges(UWidget* WidgetObject,
	const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const
{
	// 将传入的控件强制转换为自定义的属性集控件。
	UScAttributeWidget* AttributeWidget = Cast<UScAttributeWidget>(WidgetObject);
	// 检查控件是否为ScAttributeWidget。
	if (!IsValid(AttributeWidget)) return;
	// 再检查属性是否已匹配，是则调用更新函数。
	if (!AttributeWidget->MatchesAttributes(Pair)) return;
	// 更新初始值，AttributeSet需要使用原始指针。
	AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get(), 0.0f);
	// 把更新函数绑定到AbilitySystemComponent组件中的属性值变化委托里。
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Key).AddLambda([this, AttributeWidget, &Pair](const FOnAttributeChangeData& AttributeChangeData)
	{
		// 游戏中，当属性值变化时，再次更新控件。
		AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get(), AttributeChangeData.OldValue);
	});
}
