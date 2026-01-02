// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "AbilitySystem/Abilities/Player/ScPlayerPrimary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/ScGameplayTags.h"

void UScPlayerPrimary::SendHitReactEventToActors(const TArray<AActor*> ActorsHit)
{
	for (AActor* Actor : ActorsHit)
	{
		// 发送GameplayEvent。
		FGameplayEventData Payload;
		// 将重叠测试的发起者设置为技能拥有者的Avatar，然后传递Payload，Instigator（发起者）是Payload中的一员。
		// 这样当敌人接收到这个GameplayEvent时就可以找到此技能的发起者。
		Payload.Instigator = GetAvatarActorFromActorInfo();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, ScGameplayTags::Events::Enemy::HitReact, Payload);
	}
}