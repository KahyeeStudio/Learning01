// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "AbilitySystem/Abilities/ScGameplayAbilityBase.h"


void UScGameplayAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);	
	/*if (bDrawDebugs && IsValid(GEngine))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, FString::Printf(TEXT("%s Activated."), *GetName()));
	}*/
}