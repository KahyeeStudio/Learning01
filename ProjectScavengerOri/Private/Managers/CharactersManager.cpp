// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "Managers/CharactersManager.h"
#include "Characters/ScCharacterBase.h"
#include "Characters/ScPlayerCharacter.h"
#include "Characters/ScEnemyCharacter.h"

void UCharactersManager::RegCharacter(AScCharacterBase* InCharacter)
{
	if (!IsValid(InCharacter)) return;
	CachedCharacters.AddUnique(InCharacter);
	//UE_LOG(LogTemp, Warning, TEXT("%s 已注册到CachedCharacters。"), *InCharacter->GetName());
}

void UCharactersManager::DeregCharacter(AScCharacterBase* InCharacter)
{
	// Remove本身就能安全处理不存在的情况。
	CachedCharacters.Remove(InCharacter);
}

TArray<AScCharacterBase*> UCharactersManager::BP_GetCachedCharacters()
{
	TArray<AScCharacterBase*> OutArray;
	OutArray.Reserve(CachedCharacters.Num());
	for (const TWeakObjectPtr<AScCharacterBase>& BaseCharacter : CachedCharacters)
	{
		// 检查弱指针是否依然有效。
		if (BaseCharacter.IsValid())
		{
			OutArray.Add(BaseCharacter.Get());
		}
	}
	return OutArray;
}

void UCharactersManager::RegPlayerCharacter(AScPlayerCharacter* InPlayerCharacter)
{
	if (!IsValid(InPlayerCharacter)) return;
	PlayerCharacters.AddUnique(InPlayerCharacter);
	//UE_LOG(LogTemp, Warning, TEXT("%s 已注册到PlayerCharacters。"), *InPlayerCharacter->GetName());
}

void UCharactersManager::DeregPlayerCharacter(AScPlayerCharacter* InPlayerCharacter)
{
	PlayerCharacters.Remove(InPlayerCharacter);
}

TArray<AScPlayerCharacter*> UCharactersManager::BP_GetPlayerCharacters()
{
	TArray<AScPlayerCharacter*> OutArray;
	OutArray.Reserve(PlayerCharacters.Num());
	for (const TWeakObjectPtr<AScPlayerCharacter>& PlayerCharacter : PlayerCharacters)
	{
		// 检查弱指针是否依然有效。
		if (PlayerCharacter.IsValid())
		{
			OutArray.Add(PlayerCharacter.Get());
		}
	}
	return OutArray;
}

void UCharactersManager::RegPlayerPlayerCharacter(AScPlayerCharacter* InPlayerCharacter)
{
	if (!IsValid(InPlayerCharacter)) return;
	CurrentPlayerCharacter = InPlayerCharacter;
}

void UCharactersManager::DeregCurrentPlayerCharacter(AScPlayerCharacter* InPlayerCharacter)
{
	if (!IsValid(InPlayerCharacter)) return;
	CurrentPlayerCharacter.Reset();
}

AScPlayerCharacter* UCharactersManager::BP_GetCurrentPlayerCharacter()
{
	if (!CurrentPlayerCharacter.IsValid()) return nullptr;
	return CurrentPlayerCharacter.Get();
}

void UCharactersManager::RegEnemyCharacter(AScEnemyCharacter* InEnemyCharacter)
{
	if (!IsValid(InEnemyCharacter)) return;
	EnemyCharacters.AddUnique(InEnemyCharacter);
	//UE_LOG(LogTemp, Warning, TEXT("%s 已注册到EnemyCharacters。"), *InEnemyCharacter->GetName());
}

void UCharactersManager::DeregEnemyCharacter(AScEnemyCharacter* InEnemyCharacter)
{
	EnemyCharacters.Remove(InEnemyCharacter);
}

TArray<AScEnemyCharacter*> UCharactersManager::BP_GetEnemyCharacters()
{
	TArray<AScEnemyCharacter*> OutArray;
	OutArray.Reserve(EnemyCharacters.Num());
	for (const TWeakObjectPtr<AScEnemyCharacter>& EnemyCharacter : EnemyCharacters)
	{
		// 检查弱指针是否依然有效。
		if (EnemyCharacter.IsValid())
		{
			OutArray.Add(EnemyCharacter.Get());
		}
	}
	return OutArray;
}
