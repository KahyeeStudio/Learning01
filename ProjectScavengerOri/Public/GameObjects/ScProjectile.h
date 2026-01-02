// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScProjectile.generated.h"

/**
 * 投射物基类。
 */

class UProjectileMovementComponent;
class UGameplayEffect;

UCLASS()
class PROJECTSCAVENGER_API AScProjectile : public AActor
{
	GENERATED_BODY()

public:
	
	AScProjectile();
		
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	/** 需要减少血量时，此变量须为正值（因已在自建函数库的静态函数中使用负号格式化伤害数值）。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scavenger|Damage", meta = (ExposeOnSpawn))
	float Damage = -10.0f;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Scavenger|Projectile")
	void SpawnImpactEffects();
	
private:
	
	UPROPERTY(VisibleAnywhere, Category = "Scavenger|Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
	UPROPERTY(EditDefaultsOnly, Category = "Scavenger|Damage")
	TSubclassOf<UGameplayEffect> DamageEffect;
	
};
