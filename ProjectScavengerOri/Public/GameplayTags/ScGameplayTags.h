// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

/**
 * 游戏标签的C++类。
 */

namespace ScGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Blank);
	namespace Abilities
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ActivateOnGiven);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(BlockHitReact);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Death);
		namespace Player
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Secondary);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tertiary);
		}
		namespace Enemy
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
		}
	}
	namespace Cooldown
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Secondary);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tertiary);
	}
	namespace Events
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(KillScored);
		namespace Player
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Death);
		}
		namespace Enemy
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(EndAttack);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(MeleeTraceHit);
		}
	}
	namespace Status
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Alive);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dead);
	}
	namespace SetByCaller
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Melee);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Projectile);
		namespace Damage
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Secondary);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tertiary);
		}
		namespace Cooldown
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Secondary);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tertiary);
		}
		namespace Cost
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Mana);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Health);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Energy);
		}

	}
}