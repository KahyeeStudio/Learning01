// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "Notifies/ScAnsEnemyMeleeAttack.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetTraceUtils.h"
#include "Characters/ScPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/ScGameplayTags.h"

void UScAnsEnemyMeleeAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
	if (!IsValid(MeshComp)) return;
	if (!IsValid(MeshComp->GetOwner())) return;
	
	TArray<FHitResult> Hits = PerformSphereTrace(MeshComp);
	SendEventsToActors(MeshComp, Hits);	
}

TArray<FHitResult> UScAnsEnemyMeleeAttack::PerformSphereTrace(USkeletalMeshComponent* MeshComp) const
{	
	TArray<FHitResult> OutHits;	
	const FTransform SocketTransform = MeshComp->GetSocketTransform(SocketName);
	const FVector Start = SocketTransform.GetLocation();
	const FVector ExtendedSocketDirection = UKismetMathLibrary::GetForwardVector(SocketTransform.GetRotation().Rotator()) * SocketExtensionOffset;
	const FVector End = Start - ExtendedSocketDirection;	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(MeshComp->GetOwner());
	// 优化扫描通道。
	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);
	// 获取World，并选择获取失败时的处理模式。
	UWorld* World = GEngine->GetWorldFromContextObject(MeshComp, EGetWorldErrorMode::LogAndReturnNull);
	// World无效时返回空数组。
	if (!IsValid(World)) return OutHits;
	bool const bHit = World->SweepMultiByChannel(OutHits, Start, End, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(SphereTraceRadius), Params, ResponseParams);
	// 绘制Debug球体追踪。
	if (bDrawDebug)
	{
		// UE默认球体颜色为红色，命中颜色为绿色，此处暂时修改为球体蓝色，命中红色。
		DrawDebugSphereTraceMulti(World, Start, End, SphereTraceRadius, EDrawDebugTrace::ForDuration, bHit, OutHits, FColor::Cyan, FColor::Red, 5.0f);
	}
	return OutHits;
}

void UScAnsEnemyMeleeAttack::SendEventsToActors(USkeletalMeshComponent* MeshComp, const TArray<FHitResult>& Hits) const
{	
	for (const FHitResult& Hit : Hits)
	{
		// 可修改为基础角色类，以便实现友军伤害或敌人之间互相伤害。
		AScPlayerCharacter* PlayerCharacter = Cast<AScPlayerCharacter>(Hit.GetActor());
		if (!IsValid(PlayerCharacter)) continue;
		if (!PlayerCharacter->IsAlive()) continue;
		UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
		if (!IsValid(ASC)) continue;
		
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		// 循环内部，把Hit传入句柄，切记不要传错！
		ContextHandle.AddHitResult(Hit);
		
		FGameplayEventData Payload;
		Payload.Target = PlayerCharacter;
		Payload.ContextHandle = ContextHandle;
		Payload.Instigator = MeshComp->GetOwner();
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), ScGameplayTags::Events::Enemy::MeleeTraceHit, Payload);
	}
}
