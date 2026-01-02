// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "AttributeSet.h"
#include "ScWidgetComponent.generated.h"

class AScCharacterBase;
class UScAbilitySystemComponent;
class UScAttributeSet;
class UAbilitySystemComponent;

/**
 * 负责监听属性集中的属性变化。
 */

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTSCAVENGER_API UScWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:

protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Scavenger")
	TMap<FGameplayAttribute, FGameplayAttribute> AttributeMap;

private:

	TWeakObjectPtr<AScCharacterBase> ScCharacter;
	TWeakObjectPtr<UScAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<UScAttributeSet> AttributeSet;

	void InitAbilitySystemData();
	bool IsASCInitialized() const;
	void InitializeAttributeDelegate();
	
	UFUNCTION()
	void OnASCInitialized(UAbilitySystemComponent* ASC, UAttributeSet* AS);

	// 完成初始化后，监听属性变化，更新Widget。
	UFUNCTION()
	void BindToAttributeChanges();
	
	// 属性值变化时更新控件。
	void BindWidgetToAttributeChanges(UWidget* WidgetObject, const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const;
	
};
