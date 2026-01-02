// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "Characters/ScCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Managers/CharactersManager.h"

namespace ScTags
{
	const FName Player = FName("Player");
}

AScCharacterBase::AScCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// Tick and refresh bone transforms whether rendered or not - for bone updates on a dedicated sever.
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

}

void AScCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, bAlive);
	DOREPLIFETIME(ThisClass, bIsBeingLaunched);
}

UAbilitySystemComponent* AScCharacterBase::GetAbilitySystemComponent() const
{
	return nullptr;
}

void AScCharacterBase::HandleRespawn()
{
	bAlive = true;
}

void AScCharacterBase::ResetAttributes()
{
	checkf(IsValid(ResetAttributesEffect), TEXT("ResetAttributesEffect未设置。"));
	if (!IsValid(GetAbilitySystemComponent())) return;
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(ResetAttributesEffect, 1.0f, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AScCharacterBase::StopMovementUntilLanded()
{
	UE_LOG(LogTemp, Warning, TEXT("%s of %s Class: StopMovementUntilLanded()未实现。"), *GetName(), *GetClass()->GetName());
}

void AScCharacterBase::GiveStartupAbilities()
{
	if (IsValid(GetAbilitySystemComponent()))
	{
		for (const auto& Ability : StartupAbilities)
		{
			// 构造GameplayAbilitySpec能力规格。
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability);
			GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilitySystemComponent无效。"));
		return;
	}
}

void AScCharacterBase::InitializeAttributes() const
{
	checkf(IsValid(InitializeAttributesEffect), TEXT("InitializeAttributesEffect未设置。"));
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InitializeAttributesEffect, 1.0f, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AScCharacterBase::OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData)
{
	// 将生命值变化绑定到委托上，能力系统组件会进行广播，处理这个广播的数据。
	if (AttributeChangeData.NewValue <= 0.0f)
	{
		HandleDeath();
	}
}

void AScCharacterBase::HandleDeath()
{
	bAlive = false;	
	/*if (IsValid(GEngine))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("%s had dead!"), *GetName()));
	}*/
}

void AScCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	// 注册到管理器。
	if (UCharactersManager* Subsystem = GetWorld()->GetSubsystem<UCharactersManager>())
	{
		//UE_LOG(LogTemp, Warning, TEXT("开始注册 %s。"), *this->GetName());
		Subsystem->RegCharacter(this);		
	}
}

void AScCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 反注册放在 Super::EndPlay 之前，这样Subsystem里还能拿到一个有效的this指针做清理。
	if (UCharactersManager* Subsystem = GetWorld()->GetSubsystem<UCharactersManager>())
	{
		Subsystem->DeregCharacter(this);
	}
	Super::EndPlay(EndPlayReason);
}
