// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "Libraries/ScGASFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ScAttributeSet.h"
#include "Characters/ScCharacterBase.h"
#include "Characters/ScEnemyCharacter.h"
#include "Characters/ScPlayerCharacter.h"
#include "GameplayTags/ScGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/CharactersManager.h"
#include "Engine/OverlapResult.h"

EHitDirection UScGASFunctionLibrary::GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator)
{
	// 利用点积来计算前后。
	const float Dot = FVector::DotProduct(TargetForward, ToInstigator);
	if (Dot < -0.5f)
	{
		return EHitDirection::Back;
	}
	if (Dot < 0.5f)
	{
		// 利用叉乘来计算左右。
		const FVector Cross = FVector::CrossProduct(TargetForward, ToInstigator);
		if (Cross.Z < 0.0f)
		{
			return EHitDirection::Left;
		}
		return EHitDirection::Right;
	}
	return EHitDirection::Front;
}

FName UScGASFunctionLibrary::GetHitDirectionFName(const EHitDirection& HitDirection)
{
	switch (HitDirection)
	{
	case EHitDirection::Left: return FName("Left");
	case EHitDirection::Right: return FName("Right");
	case EHitDirection::Front: return FName("Front");
	case EHitDirection::Back: return FName("Back");
	default: return FName("None");
	}
}

FClosestActorWithTagResult UScGASFunctionLibrary::FindClosestActorWithTag(AActor* AvatarActor, const FVector& Origin, const FName& Tag)
{
	TArray<AActor*> ActorsWithTag;
	// 先尝试从管理缓存中收集带Tag的角色。
	UCharactersManager* CharactersManager = AvatarActor->GetWorld()->GetSubsystem<UCharactersManager>();
	if (IsValid(CharactersManager))
	{
		const TArray<TWeakObjectPtr<AScCharacterBase>>& TempBaseChars = CharactersManager->GetCachedCharacters();
		for (const TWeakObjectPtr<AScCharacterBase>& Element : TempBaseChars)
		{
			// 需要拿到原始指针才可调用成员函数。
			AScCharacterBase* BaseCharacter = Element.Get();
			// 先检查弱指针有效性，防止崩溃。
			if (!IsValid(BaseCharacter)) continue;
			// 过滤已死亡的角色。
			if (!BaseCharacter->IsAlive()) continue;
			// 判断HasTag，此Tag非GameplayTag。
			if (!BaseCharacter->ActorHasTag(Tag)) continue;
			ActorsWithTag.AddUnique(BaseCharacter);
		}
		// 如果管理器缓存未收集到任何带Tag且Alive的角色，则用GetAllActorsWithTag兜底。
		TWeakObjectPtr<AScPlayerCharacter> WeakCurPlayer = CharactersManager->GetCurrentPlayerCharacter();
		if (WeakCurPlayer.IsValid())
		{
			// 需要拿到原始指针才可调用成员函数。
			AScPlayerCharacter* CurPlayer = WeakCurPlayer.Get();
			// 防止玩家角色死亡后敌人依然搜索。
			if (CurPlayer->IsAlive())
			{
				if (ActorsWithTag.IsEmpty())
				{
					UGameplayStatics::GetAllActorsWithTag(AvatarActor, Tag, ActorsWithTag);
					UE_LOG(LogTemp, Warning, TEXT("ScGASFunctionLibrary->FindClosestActorWithTag, Executed GetAllActorsWithTag."));
				}
			}
		}
	}
	AScCharacterBase* EnemyCharBase = Cast<AScCharacterBase>(AvatarActor);
	// 设置两个内部变量。
	float ClosestDistanceSq = (EnemyCharBase->SearchRange) > 0.0f ? FMath::Square(EnemyCharBase->SearchRange) : TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;
	// 寻找最近的角色。
	for (AActor* Actor : ActorsWithTag)
	{
		if (!IsValid(Actor)) continue;
		AScCharacterBase* BaseCharacter = Cast<AScCharacterBase>(Actor);
		// 如果类型转换失败或找到的Actor已死亡，则跳过此Actor。
		if (!IsValid(BaseCharacter)) continue;
		if (!BaseCharacter->IsAlive()) continue;
		// 比较距离远近时可以不开平方，减少计算量。
		const float DistSq = FVector::DistSquared(Origin, Actor->GetActorLocation());
		if (DistSq < ClosestDistanceSq)
		{
			ClosestDistanceSq = DistSq;
			ClosestActor = Actor;
		}
	}
	// 构造输出结果。
	FClosestActorWithTagResult Result;
	Result.Actor = ClosestActor;
	// 最后输出距离时再开平方，这样只开一次即可。
	Result.Distance = IsValid(ClosestActor) ? FMath::Sqrt(ClosestDistanceSq) : -1.0f;
	return Result;
}

void UScGASFunctionLibrary::SendDamageEventToPlayer(AActor* Target, const TSubclassOf<UGameplayEffect> DamageEffect,	FGameplayEventData& Payload, const FGameplayTag& DataTag, float Damage, UObject* OptionalParticleSystem, bool bSendHitReactEvent)
{
	AScCharacterBase* BaseCharacter = Cast<AScCharacterBase>(Target);
	if (!IsValid(BaseCharacter)) return;
	if (!BaseCharacter->IsAlive()) return;
	// 获取属性集。
	UScAttributeSet* AttributeSet = Cast<UScAttributeSet>(BaseCharacter->GetAttributeSet());
	if (!IsValid(AttributeSet)) return;
	// 判断本次伤害是否致命。
	const bool bLethal = AttributeSet->GetHealth() - Damage <= 0.0f;
	// 致命则发送死亡事件，不致命则按参数发送受击事件。
	FGameplayTag EventTag;
	if (bLethal)
	{
		EventTag = ScGameplayTags::Events::Player::Death;
	}
	else
	{
		EventTag = bSendHitReactEvent ? ScGameplayTags::Events::Player::HitReact : ScGameplayTags::Blank;
	}
	// 将可选粒子特效加入Payload。
	Payload.OptionalObject = OptionalParticleSystem;
	// 发送游戏事件。
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(BaseCharacter, EventTag, Payload);	
	// 伤害。
	UAbilitySystemComponent* TargetASC = BaseCharacter->GetAbilitySystemComponent();
	if (!IsValid(TargetASC)) return;
	FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffect, 1.0f, ContextHandle);
	// 此处Damage前加了负号，防止传入的值为正值。（注：此处可能出现Bug。）
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DataTag, -Damage);
	// 应用技能效果，记得使用*解引用。
	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());	
}

TArray<AActor*> UScGASFunctionLibrary::HitBoxOverlapTest(AActor* AvatarActor, float HitBoxRadius,  float HitBoxForwardOffset, float HitBoxElevationOffset, bool bDrawDebugs)
{
	// 检查指针，无效则返回空数组。
	if (!IsValid(AvatarActor)) return TArray<AActor*>();
	FCollisionQueryParams QueryParams;
	// 忽略自身。
	QueryParams.AddIgnoredActor(AvatarActor);
	// 配置响应参数。
	FCollisionResponseParams ResponseParams;
	// 先忽略所有通道。
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	// 然后添加Pawn通道，以实现仅检测Pawn之间的重叠事件。
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);

	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(HitBoxRadius);

	// 碰撞盒体的位置。
	const FVector Forward = AvatarActor->GetActorForwardVector() * HitBoxForwardOffset;
	const FVector HitBoxLocation = AvatarActor->GetActorLocation() + Forward + FVector(0.0f, 0.0f, HitBoxElevationOffset);
	
	// 获取World，并选择获取失败时的处理模式。
	UWorld* World = GEngine->GetWorldFromContextObject(AvatarActor, EGetWorldErrorMode::LogAndReturnNull);
	// 检查World，无效则返回空数组。
	if (!IsValid(World)) return TArray<AActor*>();
	// C++中的重叠测试是针对World Object的，参数：结果过滤，碰撞盒体位置，碰撞盒体旋转，追踪通道，形状，碰撞查询参数，响应参数。
	World->OverlapMultiByChannel(OverlapResults, HitBoxLocation, FQuat::Identity, ECC_Visibility, Sphere, QueryParams, ResponseParams);

	TArray<AActor*> ActorsHit;
	for (const FOverlapResult& Result : OverlapResults)
	{
		AScCharacterBase* BaseCharacter = Cast<AScCharacterBase>(Result.GetActor());
		if (!IsValid(BaseCharacter)) continue;
		if (!BaseCharacter->IsAlive()) continue;
		ActorsHit.AddUnique(Result.GetActor());
	}
	// 绘制Debug球体。
	if (bDrawDebugs)
	{
		DrawHitBoxOverlapDebugs(AvatarActor, OverlapResults, HitBoxLocation, HitBoxRadius);		
	}
	return ActorsHit;
}

void UScGASFunctionLibrary::DrawHitBoxOverlapDebugs(const UObject* WorldContextObject, const TArray<FOverlapResult>& OverlapResults, const FVector& HitBoxLocation, const float HitBoxRadius)
{
	// 获取World，并选择获取失败时的处理模式。
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	// 检查World，无效则返回。
	if (!IsValid(World)) return;
	// 整体。
	DrawDebugSphere(World, HitBoxLocation, HitBoxRadius, 16, FColor::Cyan, false, 3.0f);
	// 单个结果。
	for (const FOverlapResult& Result : OverlapResults)
	{
		if (IsValid(Result.GetActor()))
		{
			FVector DebugLocation = Result.GetActor()->GetActorLocation();
			DebugLocation.Z += 100.0f;
			DrawDebugSphere(World, DebugLocation, 30.0f, 10, FColor::Green, false, 3.0f);
		}
	}
}

void UScGASFunctionLibrary::SendDamageEventToEnemies(TArray<AActor*> Enemies, const TSubclassOf<UGameplayEffect> DamageEffect,	FGameplayEventData& Payload, const FGameplayTag& DataTag, float Damage, UObject* OptionalParticleSystem)
{
	for (AActor* Enemy : Enemies)
	{
		// 若敌人已经死亡则下一个。
		AScCharacterBase* BaseCharacter = Cast<AScCharacterBase>(Enemy);
		if (!IsValid(BaseCharacter)) continue;
		if (!BaseCharacter->IsAlive()) continue;
		// 将可选粒子特效加入Payload。
		Payload.OptionalObject = OptionalParticleSystem;
		// 伤害。
		UAbilitySystemComponent* TargetASC = BaseCharacter->GetAbilitySystemComponent();
		if (!IsValid(TargetASC)) continue;
		FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffect, 1.0f, ContextHandle);
		// 此处Damage前加了负号，防止传入的值为正值。（注：此处可能出现Bug。）
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DataTag, -Damage);
		// 应用技能效果，记得使用*解引用。
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());	
	}
}

TArray<AActor*> UScGASFunctionLibrary::ApplyKnockback(AActor* AvatarActor, const TArray<AActor*> HitActors, float InnerRadius, float OuterRadius, float LaunchForceMagnitude, float RotationAngle, bool bDrawDebugs)
{
	for (AActor* HitActor : HitActors)
	{
		// 需要将AActor转换为ACharacter。
		ACharacter* HitCharacter = Cast<ACharacter>(HitActor);
		if (!IsValid(HitCharacter) || !IsValid(AvatarActor)) return TArray<AActor*>();
		// 被击中的角色的位置。
		const FVector HitCharacterLocation = HitCharacter->GetActorLocation();
		// 技能施放者的位置。
		const FVector AvatarLocation = AvatarActor->GetActorLocation();
		// 发射方向的向量。
		const FVector ToHitActor = HitCharacterLocation - AvatarLocation;
		// 计算两者之间的距离。
		const float Distance = FVector::Dist(AvatarLocation, HitCharacterLocation);
		// 发射力度。
		float LaunchForce;
		// 距离大于外半径，无发射效果。
		if (Distance > OuterRadius) continue;
		// 距离小于等于内半径，发射力度最大。
		if (Distance <= InnerRadius)	{LaunchForce = LaunchForceMagnitude;}
		else
		{
			// 效果衰减范围的向量。
			const FVector2D FallOffRange(InnerRadius, OuterRadius);
			// 发射力度向量的区间，从最大值衰减到0。
			const FVector2D LaunchForceRange(LaunchForceMagnitude, 0.0f);
			// 实现线性衰减。
			LaunchForce = FMath::GetMappedRangeValueClamped(FallOffRange, LaunchForceRange, Distance);			
		}
		// 打印发射力度的取值。
		if (bDrawDebugs)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("击退力度：%f"), LaunchForce));
		}
		// 把指向被击退角色的向量归一化，获得方向。
		FVector KnockbackForce = ToHitActor.GetSafeNormal();
		// 把此方向水平化。
		KnockbackForce.Z = 0.0f;
		// 在此方向根部右侧放置一个轴向量，用于下一步的上下旋转。
		const FVector Right = KnockbackForce.RotateAngleAxis(90.0f, FVector::UpVector);
		// 把此方向按轴向量抬高指定角度。
		KnockbackForce = KnockbackForce.RotateAngleAxis(-RotationAngle, Right);
		// 再乘以发射力度，才能最终得到击退力度的向量。
		KnockbackForce *= LaunchForce;
		// 绘制Debug箭头。
		if (bDrawDebugs)
		{
			UWorld* World = GEngine->GetWorldFromContextObject(AvatarActor, EGetWorldErrorMode::LogAndReturnNull);
			DrawDebugDirectionalArrow(World, HitCharacterLocation, HitCharacterLocation + KnockbackForce, 100.0f, FColor::Green, false, 3.0f);
		}
		// 发射角色。
		HitCharacter->LaunchCharacter(KnockbackForce, true, true);
		// 停止角色移动。
		AScEnemyCharacter* EnemyCharacter = Cast<AScEnemyCharacter>(HitCharacter);
		if (IsValid(EnemyCharacter))
		{
			EnemyCharacter->StopMovementUntilLanded();
		}
	}
	return  HitActors;
}
