// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "GameObjects/ScProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Characters/ScPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/ScGameplayTags.h"
#include "Libraries/ScGASFunctionLibrary.h"


AScProjectile::AScProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	// 这是一个可复制的Actor。
	bReplicates = true;
	
}

void AScProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	// 将碰撞到的角色转换为玩家，然后检查有效性和是否存活。
	AScPlayerCharacter* PlayerCharacter = Cast<AScPlayerCharacter>(OtherActor);
	// 先检查PlayerCharacter是否为空，再检查PlayerCharacter其中的函数，否则解引用时会崩溃。
	if (!IsValid(PlayerCharacter)) return;
	if (!PlayerCharacter->IsAlive()) return;
	// 获取技能系统组件，检查有效性，并检查时是否具有服务器权限。
	UAbilitySystemComponent* AbilitySystemComponent = PlayerCharacter->GetAbilitySystemComponent();
	if (!IsValid(AbilitySystemComponent) || !HasAuthority()) return;
	// 创建一个Payload，为接下来使用函数库中的静态函数做准备。
	FGameplayEventData Payload;
	Payload.Instigator = this;
	Payload.Target = PlayerCharacter;
	// 使用自建的函数库中的静态函数。
	UScGASFunctionLibrary::SendDamageEventToPlayer(PlayerCharacter, DamageEffect, Payload, ScGameplayTags::SetByCaller::Projectile, Damage);
	/* 以下代码已在自建函数库中的静态函数中调用。
	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DamageEffect, 1.0f, ContextHandle);
	// 将GE中的Modifier Manitude设定为SetByCaller，设置好游戏标签，然后应用伤害变量即可修改技能伤害。
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, ScGameplayTags::SetByCaller::Projectile, Damage);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	*/
	SpawnImpactEffects();
	Destroy();
}



