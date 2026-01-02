// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ScGASFunctionLibrary.generated.h"

/**
 * 也包含GAS系统外但是与GAS相关的一些静态函数。
 */

class UGameplayEffect;
struct FGameplayEventData;
struct FGameplayTag;
struct FOverlapResult;

UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	Left,
	Right,
	Front,
	Back
};

USTRUCT(BlueprintType)
struct FClosestActorWithTagResult
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> Actor;
	
	UPROPERTY(BlueprintReadWrite)
	float Distance = 0.0f;	
};

UCLASS()
class PROJECTSCAVENGER_API UScGASFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** 根据目标朝向和攻击者来源向量，计算命中方向（例如：来自前方、后方、左侧或右侧）。
	 * @param TargetForward 被命中的目标（Target）的前向向量。
	 * @param ToInstigator 一个从目标（Target）指向攻击来源（Instigator）的向量。
	 */
	UFUNCTION(BlueprintPure, Category = "Scavenger|FunctionLibrary")
	static EHitDirection GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator);

	/** 将受击方向的枚举转换为FName，用于播放Montage。
     * @param HitDirection 受击方向的枚举。
     */
	UFUNCTION(BlueprintPure, Category = "Scavenger|FunctionLibrary")
	static FName GetHitDirectionFName(const EHitDirection& HitDirection);

	/** 寻找最近的带有指定标签的Actor，检查并过滤非存活的目标，搜索范围受角色基类中的SearchRange属性制约。
	 * 返回存活的FCloseActorWithTagResult结构体，包含找到的Actor的弱指针和距离。
	 * @param AvatarActor 执行此搜索的角色。
	 * @param Origin 搜索的起始点。
	 * @param Tag 此Tag仅为FName，非GameplayTag。
	 */
	UFUNCTION(BlueprintCallable, Category = "Scavenger|FunctionLibrary")
	static FClosestActorWithTagResult FindClosestActorWithTag(AActor* AvatarActor, const FVector& Origin, const FName& Tag);
	
	/** 对Player造成伤害，用于SetByCaller标签，若本次伤害导致角色死亡时发送死亡事件，否则发送受击反应事件。
	 * @param DataTag 用于SetByCaller Magnitude设置伤害的游戏标签。
	 * @param Damage 伤害的数值，必须为正。
	 * @param bSendHitReactEvent 该伤害是否触发受击反应事件。
	 */
	UFUNCTION(BlueprintCallable, Category = "Scavenger|FunctionLibrary")
	static void SendDamageEventToPlayer(AActor* Target, const TSubclassOf<UGameplayEffect> DamageEffect, UPARAM(ref) FGameplayEventData& Payload, const FGameplayTag& DataTag, float Damage, UObject* OptionalParticleSystem = nullptr, bool bSendHitReactEvent = true);
	
	/** C++碰撞盒体重叠测试。
     * @return 返回OverlapResult中的Actor。
     */
	UFUNCTION(BlueprintCallable, Category = "Scavenger|FunctionLibrary")
	static TArray<AActor*> HitBoxOverlapTest(AActor* AvatarActor, float HitBoxRadius, float HitBoxForwardOffset = 0.0f, float HitBoxElevationOffset = 0.0f, bool bDrawDebugs = false);
	
	// 此函数仅绘制Debug球体，不用于蓝图。
	static void DrawHitBoxOverlapDebugs(const UObject* WorldContextObject, const TArray<FOverlapResult>& OverlapResults, const FVector& HitBoxLocation, const float HitBoxRadius);
	
	/** 此函数仅对Enemies造成伤害，用于SetByCaller标签，不发送任何游戏事件。
	 * @param DataTag 用于SetByCaller Magnitude设置伤害的游戏标签。
	 * @param Damage 伤害的数值，必须为正。
	 */
	UFUNCTION(BlueprintCallable, Category = "Scavenger|FunctionLibrary")
	static void SendDamageEventToEnemies(TArray<AActor*> Enemies, const TSubclassOf<UGameplayEffect> DamageEffect, UPARAM(ref) FGameplayEventData& Payload, const FGameplayTag& DataTag, float Damage, UObject* OptionalParticleSystem = nullptr);
	
	/** 应用击退效果，内半径之内承受全部效果，外半径之外不承受击退效果，两者之间承受衰减效果。
	 * @param AvatarActor 技能施放者。
	 * @param HitActors 被击退者的数组。
	 * @param LaunchForceMagnitude 击退的最大力度。
	 * @param RotationAngle 击退的角度，默认为30°。
	 * @return 返回值为被击退的AActor的数组。
	 */
	UFUNCTION(BlueprintCallable, Category = "Scavenger|FunctionLibrary")
	static TArray<AActor*> ApplyKnockback(AActor* AvatarActor, const TArray<AActor*> HitActors, float InnerRadius, float OuterRadius, float LaunchForceMagnitude, float RotationAngle = 30.0f, bool bDrawDebugs = false);
};
