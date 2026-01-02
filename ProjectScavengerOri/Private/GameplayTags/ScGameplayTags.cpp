// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "GameplayTags/ScGameplayTags.h"


namespace ScGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Blank, "ScGameplayTags.Blank", "占位符，空游戏标签。");
	namespace Abilities
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateOnGiven, "ScGameplayTags.Abilities.ActivateOnGiven", "一旦被赋予则立即激活的技能标签。");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BlockHitReact, "ScGameplayTags.Abilities.BlockHitReact", "阻止受击反应的技能标签，可防止受击导致攻击被中断。");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Death, "ScGameplayTags.Abilities.Death", "死亡技能标签。");
		namespace Player
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "ScGameplayTags.Abilities.Player.Primary", "玩家主技能标签。");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Secondary, "ScGameplayTags.Abilities.Player.Secondary", "玩家二级技能标签。");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tertiary, "ScGameplayTags.Abilities.Player.Tertiary", "玩家三级技能标签。");
		}
		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attack, "ScGameplayTags.Abilities.Enemy.Attack", "敌人攻击的技能标签。");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "ScGameplayTags.Abilities.Enemy.HitReact", "敌人被击中时的技能标签。");
		}
	}
	namespace Cooldown
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "ScGameplayTags.Cooldown.Primary", "一级技能的冷却标签，需要在GA的阻止激活和GE默认值中同时配置。");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Secondary, "ScGameplayTags.Cooldown.Secondary", "二级技能的冷却标签，需要在GA的阻止激活和GE默认值中同时配置。");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tertiary, "ScGameplayTags.Cooldown.Tertiary", "三级技能的冷却标签，需要在GA的阻止激活和GE默认值中同时配置。");
	}
	namespace Events
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(KillScored, "ScGameplayTags.Events.KillScored", "击杀得分的事件标签，Character死亡后向击杀者发送此游戏事件，可用于应用各种击杀奖励，包括但不限于经验、物品、升级等。");
		namespace Player
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "ScGameplayTags.Events.Player.Primary", "玩家主技能触发时的事件标签。");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "ScGameplayTags.Events.Player.HitReact", "玩家受击反应触发时的事件标签。");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Death, "ScGameplayTags.Events.Player.Death", "玩家死亡时的事件标签。");
		}
		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(EndAttack, "ScGameplayTags.Events.Enemy.EndAttack", "敌人结束攻击的事件标签。");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "ScGameplayTags.Events.Enemy.HitReact", "敌人被击中时的事件标签。");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(MeleeTraceHit, "ScGameplayTags.Events.Enemy.MeleeTraceHit", "敌人近战攻击命中的事件标签。");
		}
	}
	namespace Status
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Alive, "ScGameplayTags.Status.Alive", "存活的状态标签。");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dead, "ScGameplayTags.Status.Dead", "死亡的状态标签。");
	}

	namespace SetByCaller
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Melee, "ScGameplayTags.SetByCaller.Melee", "近战攻击伤害的SetByCaller标签。");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Projectile, "ScGameplayTags.SetByCaller.Projectile", "投射物伤害的SetByCaller标签。");
		namespace Damage
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "ScGameplayTags.SetByCaller.Damage.Primary", "玩家1技能伤害的SetByCaller标签。");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Secondary, "ScGameplayTags.SetByCaller.Damage.Secondary", "玩家2技能伤害的SetByCaller标签。");
		}
		namespace Cooldown
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "ScGameplayTags.SetByCaller.Cooldown.Primary", "玩家1技能冷却的SetByCaller标签。");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Secondary, "ScGameplayTags.SetByCaller.Cooldown.Secondary", "玩家2技能冷却的SetByCaller标签。");
		}
		namespace Cost
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Mana, "ScGameplayTags.SetByCaller.Cost.Mana", "消耗魔法的SetByCaller标签。");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Health, "ScGameplayTags.SetByCaller.Cost.Health", "消耗声明的SetByCaller标签。");
		}		
	}
}