// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ScAnsEnemyMeleeAttack.generated.h"

/**
 * AnimNotifyState，AnimMontage能够每帧调用ReceivedNotifyTick函数，实现绘制球体追踪。
 * SendEventsToActors中被命中的Actor当前设置为玩家角色，可以修改为基础角色类，以便实现友军伤害或敌人之间互相伤害。
 */

UCLASS()
class PROJECTSCAVENGER_API UScAnsEnemyMeleeAttack : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:	
	
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	
private:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scavenger|Debugs", meta = (AllowPrivateAccess = true))
	bool bDrawDebug = true;
	
	UPROPERTY(EditAnywhere, Category = "Scavenger")
	FName SocketName = FName("FX_Trail_01_R");
	
	UPROPERTY(EditAnywhere, Category = "Scavenger")
	float SocketExtensionOffset = 40.0f;
	
	UPROPERTY(EditAnywhere, Category = "Scavenger")
	float SphereTraceRadius = 60.0f;
	
	TArray<FHitResult> PerformSphereTrace(USkeletalMeshComponent* MeshComp) const;	
	/** 函数中被命中的Actor当前设置为玩家角色，可以修改为基础角色类，以便实现友军伤害或敌人之间互相伤害。*/
	void SendEventsToActors(USkeletalMeshComponent* MeshComp, const TArray<FHitResult>& Hits) const;
};
