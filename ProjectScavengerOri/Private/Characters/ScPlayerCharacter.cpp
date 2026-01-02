// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "Characters/ScPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/ScPlayerState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ScAttributeSet.h"
#include "Managers/CharactersManager.h"

AScPlayerCharacter::AScPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	// 相机距离角色的“杆”长度，越大越远。
	CameraBoom->TargetArmLength = 600.0f;
	// 如果需要视角固定，则需要关闭摄像机臂跟随Pawn的控制旋转。
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	Tags.Add(ScTags::Player);
}

UAbilitySystemComponent* AScPlayerCharacter::GetAbilitySystemComponent() const
{
	AScPlayerState* ScPlayerState = Cast<AScPlayerState>(GetPlayerState());
	if (!IsValid(ScPlayerState)) return nullptr;
	return ScPlayerState->GetAbilitySystemComponent();
}

UAttributeSet* AScPlayerCharacter::GetAttributeSet() const
{
	AScPlayerState* ScPlayerState = Cast<AScPlayerState>(GetPlayerState());
	if (!IsValid(ScPlayerState)) return nullptr;
	return ScPlayerState->GetAttributeSet();
}

void AScPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (IsValid(GetAbilitySystemComponent()) || HasAuthority())
	{
		// Sever初始化AbilityActorInfo。
		GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
		// 广播。
		OnASCInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());
		// 赋予能力。
		GiveStartupAbilities();
		// 初始化属性。
		InitializeAttributes();		
		// 绑定监听生命值的委托。
		UScAttributeSet* ScAttributeSet = Cast<UScAttributeSet>(GetAttributeSet());
		if (IsValid(ScAttributeSet))
		{
			// 传入的参数要使用属性而不是值。
			GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(ScAttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter->GetAbilitySystemComponent()无效或没有服务器权限。"));
	}
}

void AScPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (IsValid(GetAbilitySystemComponent()))
	{
		// Client初始化AbilityActorInfo。
		GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
		// 广播。
		OnASCInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());
		// 绑定监听生命值的委托。
		UScAttributeSet* ScAttributeSet = Cast<UScAttributeSet>(GetAttributeSet());
		if (IsValid(ScAttributeSet))
		{
			// 传入的参数要使用属性而不是值。
			GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(ScAttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter->GetAbilitySystemComponent()无效。"));
	}
}

void AScPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (UCharactersManager* Subsystem = GetWorld()->GetSubsystem<UCharactersManager>())
	{
		//UE_LOG(LogTemp, Warning, TEXT("开始注册 %s。"), *this->GetName());
		Subsystem->RegPlayerCharacter(this);		
	}
}

void AScPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 反注册放在 Super::EndPlay 之前，这样Subsystem里还能拿到一个有效的this指针做清理。
	if (UCharactersManager* Subsystem = GetWorld()->GetSubsystem<UCharactersManager>())
	{
		Subsystem->DeregPlayerCharacter(this);
	}
	Super::EndPlay(EndPlayReason);
}
