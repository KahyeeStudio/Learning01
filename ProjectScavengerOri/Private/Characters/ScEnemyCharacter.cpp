// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "Characters/ScEnemyCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ScAbilitySystemComponent.h"
#include "AbilitySystem/ScAttributeSet.h"
#include "Managers/CharactersManager.h"
#include "AIController.h"
#include "GameplayTags/ScGameplayTags.h"

AScEnemyCharacter::AScEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	AbilitySystemComponent = CreateDefaultSubobject<UScAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UScAttributeSet>("AttributeSet");
	
	// 将AIController的附身方式设置为放置在世界中或生成。
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

}

UAbilitySystemComponent* AScEnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AScEnemyCharacter::GetAttributeSet() const
{
	return AttributeSet;
}

void AScEnemyCharacter::StopMovementUntilLanded()
{
	// 此处无需输出日志，无需调用父类。
	// 设置“被发射中”变量为true。
	bIsBeingLaunched = true;
	AAIController* AIController = GetController<AAIController>();
	if (!IsValid(AIController)) return;
	AIController->StopMovement();
	// 通过委托来监听着陆，先检查委托是否已经绑定。
	if (!LandedDelegate.IsAlreadyBound(this, &ThisClass::EnableMovementOnLanded))
	{
		LandedDelegate.AddDynamic(this, &ThisClass::EnableMovementOnLanded);
	}
}

void AScEnemyCharacter::HandleDeath()
{
	Super::HandleDeath();
	// 停止敌人移动。
	AAIController* AIController = GetController<AAIController>();
	if (!IsValid(AIController)) return;
	AIController->StopMovement();
}

void AScEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(GetAbilitySystemComponent()))
	{
		GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
		// 广播。
		OnASCInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());
		if (HasAuthority())
		{
			// 赋予初始技能。
			GiveStartupAbilities();
			// 初始化属性。
			InitializeAttributes();
			// 绑定监听生命值的委托。
			UScAttributeSet* ScAttributeSet = Cast<UScAttributeSet>(GetAttributeSet());
			if (IsValid(ScAttributeSet))
			{
				// 传入的参数要使用属性而不是值。
				GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(ScAttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
			}
			// 注册到管理器。
			if (UCharactersManager* Subsystem = GetWorld()->GetSubsystem<UCharactersManager>())
			{
				//UE_LOG(LogTemp, Warning, TEXT("开始注册 %s。"), *this->GetName());
				Subsystem->RegEnemyCharacter(this);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("EnemyCharacter-GetAbilitySystemComponent()无效。"));
		return;
	}
}

void AScEnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 反注册放在 Super::EndPlay 之前，这样Subsystem里还能拿到一个有效的this指针做清理。
	if (UCharactersManager* Subsystem = GetWorld()->GetSubsystem<UCharactersManager>())
	{
		Subsystem->DeregEnemyCharacter(this);
	}
	Super::EndPlay(EndPlayReason);
}

void AScEnemyCharacter::EnableMovementOnLanded(const FHitResult& Hit)
{
	// 设置被“发射中”变量为false。
	bIsBeingLaunched = false;
	// 重新搜索目标（以发送结束攻击事件的方式，可修改或优化。目前EndAttack的接收事件中只执行了StartSearch一个函数。）。
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, ScGameplayTags::Events::Enemy::EndAttack, FGameplayEventData());
	// 解绑委托。
	LandedDelegate.RemoveAll(this);
}
