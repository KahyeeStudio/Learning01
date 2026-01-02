// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/ScCharacterBase.h"
#include "ScPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

/**
 * 玩家角色类，此类为AScCharacterBase的子类。
 */

UCLASS()
class PROJECTSCAVENGER_API AScPlayerCharacter : public AScCharacterBase
{
	GENERATED_BODY()

public:

	AScPlayerCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
protected:
	
	virtual  void BeginPlay() override;	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

};