// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "Player/ScPlayerState.h"
#include "AbilitySystem/ScAbilitySystemComponent.h"
#include "AbilitySystem/ScAttributeSet.h"

AScPlayerState::AScPlayerState()
{
	// 设置网络更新频率。
	SetNetUpdateFrequency(120.0f);

	AbilitySystemComponent = CreateDefaultSubobject<UScAbilitySystemComponent>("AbilitySystemComponent");
	// 设置为可网络复制。
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UScAttributeSet>("AttributeSet");

}

UAbilitySystemComponent* AScPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}