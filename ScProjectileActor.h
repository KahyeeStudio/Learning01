// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScProjectileActor.generated.h"

/**
 * 所有投射物的C++基类。
 */

class UPoolableComponent;
class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class A1PROJECTSCAVENGER_API AScProjectileActor : public AActor
{
	GENERATED_BODY()

public:
	
	AScProjectileActor();
	
	/** 投射物移动组件，用于处理投射物飞行。*/
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;
	
	/** 对象池组件。*/
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPoolableComponent> PoolComponent;

protected:
	
	virtual void BeginPlay() override;
	
	/** 
	 * 池化的 Actor 不要使用 Destroy 销毁对象，避免 Bug。
	 * 投射物销毁时，如果客户端未生成特效和播放音效，在调用父类前调用一次处理命中效果的函数。
	 * 
	 */
	//virtual void Destroyed() override;	
		
	/** 
	 * 如果使用了对象池，需重写寿命到期函数，把“销毁”改成“回收”。
	 */
	//virtual void LifeSpanExpired() override;
	
	/** 投射物的寿命，单位秒，如果投射物未被销毁的话，最多存续的时长。*/
	//UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 5.0f;
	
	/** 投射物的命中冲击特效。*/
	UPROPERTY(EditAnywhere, Category="Scavenger")
	TObjectPtr<UNiagaraSystem> ImpactEffect;
	
	/** 投射物的命中冲击音效。*/
	UPROPERTY(EditAnywhere, Category="Scavenger")
	TObjectPtr<USoundBase> ImpactSound;
	
	/**技能持续期间一直循环播放的音效（循环音效）。*/
	UPROPERTY(EditAnywhere, Category="Scavenger")
	TObjectPtr<USoundBase> LoopingSound;
	
	/** 球体碰撞重叠检测的回调函数。*/
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	
	/** 球体碰撞组件。*/
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereCollision;
	
	/** 用于缓存循环音效组件指针。*/
	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComp;
	
	/** 
	 * 网络同步相关：
	 * 投射物是否命中了其他Actor的标记，用于处理投射物在服务器端销毁
	 * 这个事件的网络复制发生在客户端的重叠事件发生之前的情况。
	 */
	bool bHit = false;
};