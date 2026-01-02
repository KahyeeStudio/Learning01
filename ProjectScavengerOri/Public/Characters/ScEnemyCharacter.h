// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/ScCharacterBase.h"
#include "ScEnemyCharacter.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

/**
 * 敌人类的基类。
 */

UCLASS()
class PROJECTSCAVENGER_API AScEnemyCharacter : public AScCharacterBase
{
	GENERATED_BODY()

public:

	AScEnemyCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;	
	virtual UAttributeSet* GetAttributeSet() const override;
	
	/** 接受半径，敌人AIController调用MoveTo相关函数时的参数，距离目标多近才算到达目的地。*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scavenger|AI")
	float AcceptanceRadius = 500.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scavenger|AI")
	float MinAttackDelay = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scavenger|AI")
	float MaxAttackDelay = 0.5f;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Scavenger|AI")
	float GetTimelineLength();
		
	virtual void StopMovementUntilLanded() override;
	
protected:
	
	virtual void HandleDeath() override;

	virtual void BeginPlay() override;	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
	UFUNCTION()
	void EnableMovementOnLanded(const FHitResult& Hit);

};