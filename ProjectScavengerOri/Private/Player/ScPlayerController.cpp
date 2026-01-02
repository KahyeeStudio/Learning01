// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "Player/ScPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/ScCharacterBase.h"
#include "GameplayTags/ScGameplayTags.h"

void AScPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (IsValid(InputSubsystem))
	{
		for (UInputMappingContext* Context : InputMappingContexts)
		{
			InputSubsystem->AddMappingContext(Context, 0);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InputSubsystem无效。"));
	}

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (IsValid(EnhancedInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
		EnhancedInputComponent->BindAction(PimaryAbilityAction, ETriggerEvent::Triggered, this, &ThisClass::PrimaryAbility);
		EnhancedInputComponent->BindAction(SecondaryAbilityAction, ETriggerEvent::Started, this, &ThisClass::SecondaryAbility);
		EnhancedInputComponent->BindAction(TertiaryAbilityAction, ETriggerEvent::Started, this, &ThisClass::TertiaryAbility);
		EnhancedInputComponent->BindAction(InputTestAction, ETriggerEvent::Triggered, this, &ThisClass::InputTest);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnhancedInputComponent无效。"));
	}
}

void AScPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// 如果摄像机臂的bUsePawnControlRotation = true，则玩家控制器中的ControllRotation会覆盖PlayerCharacter中的摄像机角度。
	FRotator CustomControlRotation = GetControlRotation();
	// 把Pitch改成 -60°，也就是俯视60度。
	CustomControlRotation.Pitch = -30.0f;
	// 如果想开局时角色面对世界X轴，可以顺便设一下Yaw。
	CustomControlRotation.Yaw = 0.0f;
	SetControlRotation(CustomControlRotation);
}

void AScPlayerController::Jump()
{
	if (!IsValid(GetCharacter())) return;
	if (!IsAlive()) return;
	GetCharacter()->Jump();
}

void AScPlayerController::StopJumping()
{
	if (!IsValid(GetCharacter())) return;
	if (!IsAlive()) return;
	GetCharacter()->StopJumping();
}

void AScPlayerController::Move(const FInputActionValue& Value)
{
	if (!IsValid(GetPawn())) return;
	if (!IsAlive()) return;
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator YawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	GetPawn()->AddMovementInput(ForwardDirection, MovementVector.Y);
	GetPawn()->AddMovementInput(RightDirection, MovementVector.X);
}

void AScPlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void AScPlayerController::PrimaryAbility()
{
	ActivateAbility(ScGameplayTags::Abilities::Player::Primary);
	//UE_LOG(LogTemp, Warning, TEXT("PrimaryAbility。"));
}

void AScPlayerController::SecondaryAbility()
{
	ActivateAbility(ScGameplayTags::Abilities::Player::Secondary);
	//UE_LOG(LogTemp, Warning, TEXT("SecondaryAbility。"));
}

void AScPlayerController::TertiaryAbility()
{
	ActivateAbility(ScGameplayTags::Abilities::Player::Tertiary);
	//UE_LOG(LogTemp, Warning, TEXT("TertiaryAbility。"));
}

void AScPlayerController::InputTest()
{
	UE_LOG(LogTemp, Warning, TEXT("InputTest函数被调用。"));
}

void AScPlayerController::ActivateAbility(const FGameplayTag& AbilityTag) const
{
	// 激活技能时如果玩家不处于存活状态，则返回。
	if (!IsAlive()) return;
	UAbilitySystemComponent* TempASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetCharacter());
	if (!IsValid(TempASC)) return;
	TempASC->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer());
}

bool AScPlayerController::IsAlive() const
{
	AScCharacterBase* BaseCharacter = Cast<AScCharacterBase>(GetCharacter());
	 if (!BaseCharacter) return false;
	return BaseCharacter->IsAlive();
}
