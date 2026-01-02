// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ScPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
struct FGameplayTag;

/**
 * PlayerController的基类。
 */

UCLASS()
class PROJECTSCAVENGER_API AScPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Scavenger|Input")
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;

	UPROPERTY(EditDefaultsOnly, Category = "Scavenger|Input|Movement")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Scavenger|Input|Movement")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Scavenger|Input|Movement")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Scavenger|Input|Abilities")
	TObjectPtr<UInputAction> PimaryAbilityAction;

	UPROPERTY(EditDefaultsOnly, Category = "Scavenger|Input|Abilities")
	TObjectPtr<UInputAction> SecondaryAbilityAction;

	UPROPERTY(EditDefaultsOnly, Category = "Scavenger|Input|Abilities")
	TObjectPtr<UInputAction> TertiaryAbilityAction;

	UPROPERTY(EditDefaultsOnly, Category = "Scavenger|Input|Movement")
	TObjectPtr<UInputAction> InputTestAction;

	void Jump();
	void StopJumping();
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void PrimaryAbility();
	void SecondaryAbility();
	void TertiaryAbility();
	void InputTest();

	void ActivateAbility(const FGameplayTag& AbilityTag) const;
	
	bool IsAlive() const;

};