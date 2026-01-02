// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "AbilitySystem/ScAbilitySystemComponent.h"

#include <string>

#include "ModuleDescriptor.h"
#include "GameplayTags/ScGameplayTags.h"

void UScAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);
	HandleAutoActivatedAbility(AbilitySpec);
}

void UScAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	for (const auto& AbilitySpec : GetActivatableAbilities())
	{
		HandleAutoActivatedAbility(AbilitySpec);
	}
}

void UScAbilitySystemComponent::SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level)
{
	// 设置技能等级要在服务器上进行，检查权限的同时记得检查AvatarActor的有效性。
	if (IsValid(GetAvatarActor()) && GetAvatarActor()->HasAuthority())
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass))
		{
			AbilitySpec->Level = Level;
			// 在网络更新时，标记为Dirty的属性会被同步。
			MarkAbilitySpecDirty(*AbilitySpec);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AbilitySpec无效，请确认技能类是否为蓝图类。"))
		}
	}
}

void UScAbilitySystemComponent::AddAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Value)
{
	// 设置技能等级要在服务器上进行，检查权限的同时记得检查AvatarActor的有效性。
	if (IsValid(GetAvatarActor()) && GetAvatarActor()->HasAuthority())
	{		
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass))
		{
			// Level默认为1，将当前技能等级加1。
			AbilitySpec->Level += Value;
			// 在网络更新时，标记为Dirty的属性会被同步。
			MarkAbilitySpecDirty(*AbilitySpec);			
			//UE_LOG(LogTemp, Warning, TEXT("New Ability Level: %d"), AbilitySpec->Level)
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AbilitySpec无效，请确认技能类是否为蓝图类。"))
		}
	}
}

void UScAbilitySystemComponent::HandleAutoActivatedAbility(const FGameplayAbilitySpec& AbilitySpec)
{
	if (IsValid(AbilitySpec.Ability))
	{
		for (const FGameplayTag& Tag : AbilitySpec.Ability->GetAssetTags())
		{
			if (Tag.MatchesTagExact(ScGameplayTags::Abilities::ActivateOnGiven))
			{
				TryActivateAbility(AbilitySpec.Handle);
				return;
			}
		}
	}
	else
	{
		return;
	}
}