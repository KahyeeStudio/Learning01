// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "AbilitySystem/Abilities/Enemy/ScEnemyHitReact.h"

void UScEnemyHitReact::CacheHitDirectionVectors(AActor* Instigator)
{
	// 受击者的前方。
	AvatarForward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	// 攻击发起者的位置。
	const FVector InstigatorLocation = Instigator->GetActorLocation();
	// 受击者的位置。
	const FVector AvatarLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	// 减。
	ToInstigator = InstigatorLocation - AvatarLocation;
	// 标准化。
	ToInstigator.Normalize();
}
