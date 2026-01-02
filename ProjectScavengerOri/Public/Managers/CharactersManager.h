// Copyright (C) 2026 Kahyee Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CharactersManager.generated.h"

/**
 * 角色管理器。
 */

class AScCharacterBase;
class AScPlayerCharacter;
class AScEnemyCharacter;

UCLASS()
class PROJECTSCAVENGER_API UCharactersManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Managers")
	void RegCharacter(AScCharacterBase* InCharacter);
	
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Managers")
	void DeregCharacter(AScCharacterBase* InCharacter);
	
	const TArray<TWeakObjectPtr<AScCharacterBase>>& GetCachedCharacters() const { return CachedCharacters; }
	
	/** 蓝图可用函数，返回所有已缓存且有效的ScCharacterBase。*/
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Managers", meta = (DisplayName = "Get Cached Characters"))
	TArray<AScCharacterBase*> BP_GetCachedCharacters();
	
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Managers")
	void RegPlayerCharacter(AScPlayerCharacter* InPlayerCharacter);
	
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Managers")
	void DeregPlayerCharacter(AScPlayerCharacter* InPlayerCharacter);
	
	const TArray<TWeakObjectPtr<AScPlayerCharacter>>& GetPlayerCharacters() const { return PlayerCharacters; }
	/** 蓝图可用函数，返回所有已缓存且有效的ScPlayerCharacter（玩家）。*/
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Managers", meta = (DisplayName = "Get Player Characters"))
	TArray<AScPlayerCharacter*> BP_GetPlayerCharacters();
	
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Managers")
	void RegPlayerPlayerCharacter(AScPlayerCharacter* InPlayerCharacter);
	
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Managers")
	void DeregCurrentPlayerCharacter(AScPlayerCharacter* InPlayerCharacter);
	
	const TWeakObjectPtr<AScPlayerCharacter>& GetCurrentPlayerCharacter() const { return CurrentPlayerCharacter; }
	/** 蓝图可用函数，返回已缓存且有效的当前玩家。*/
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Managers", meta = (DisplayName = "Get Current Player Character"))
	AScPlayerCharacter* BP_GetCurrentPlayerCharacter();
	
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Managers")
	void RegEnemyCharacter(AScEnemyCharacter* InEnemyCharacter);
	
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Managers")
	void DeregEnemyCharacter(AScEnemyCharacter* InEnemyCharacter);
	
	const TArray<TWeakObjectPtr<AScEnemyCharacter>>& GetEnemyCharacters() const { return EnemyCharacters; }
	/** 蓝图可用函数，返回所有已缓存且有效的ScEnemyCharacter（敌人）。*/
	UFUNCTION(BlueprintCallable, Category = "Scavenger|Managers", meta = (DisplayName = "Get Enemy Characters"))
	TArray<AScEnemyCharacter*> BP_GetEnemyCharacters();
	
private:
	
	UPROPERTY()
	TArray<TWeakObjectPtr<AScCharacterBase>> CachedCharacters;
	
	UPROPERTY()
	TArray<TWeakObjectPtr<AScPlayerCharacter>> PlayerCharacters;
	
	UPROPERTY()
	TWeakObjectPtr<AScPlayerCharacter> CurrentPlayerCharacter;
	
	UPROPERTY()
	TArray<TWeakObjectPtr<AScEnemyCharacter>> EnemyCharacters;
};
