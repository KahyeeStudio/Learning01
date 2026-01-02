// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "AbilitySystem/AbilityTasks/ScWaitGameplayEvent.h"

UScWaitGameplayEvent* UScWaitGameplayEvent::WaitGameplayEventToActorProxy(AActor* TargetActor, FGameplayTag EventTag,
	bool OnlyTriggerOnce, bool OnlyMatchExact)
{
	UScWaitGameplayEvent* MyObj = NewObject<UScWaitGameplayEvent>();
	MyObj->SetAbilityActor(TargetActor);
	MyObj->Tag = EventTag;
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;
	MyObj->OnlyMatchExact = OnlyMatchExact;
	return MyObj;
}

void UScWaitGameplayEvent::StartActivation()
{
	Activate();
}
