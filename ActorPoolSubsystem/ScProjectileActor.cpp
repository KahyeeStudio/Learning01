// Copyright (C) 2026 Kahyee Studio. All rights reserved.


#include "GameObjects/ScProjectileActor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include  "Managers/PoolableComponent.h"
#include "Managers/PoolSubsystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"


AScProjectileActor::AScProjectileActor()
{
	PrimaryActorTick.bCanEverTick = false;
	// 打开网络复制，这样可以在服务器生成该类，在服务器移动该类。
	bReplicates = true;
	// 创建球体碰撞组件。
	SphereCollision = CreateDefaultSubobject<USphereComponent>("SphereCollision");	
	// ReSharper disable once CommentTypo
	// 必须把碰撞体设置为根组件，如果根组件为默认的场景组件的话，投射物可能不会动（B站的UP主SoulKeY_Hiigara说的）？
	SetRootComponent(SphereCollision);	
	/* 
	 * 把这个碰撞组件的“碰撞开关模式”设为 只参与查询（Query）。
	 * 具体效果：
	 * 会参与 Query：射线检测（LineTrace）、扫掠（Sweep）、Overlap 检测、OnComponentBeginOverlap / OnComponentEndOverlap、
	 * 以及带 Sweep 的移动检测都还能用。
	 * 不会参与 Physics：不会产生物理碰撞响应（不阻挡、不弹开）、不会推动/被推动、不会产生物理接触解算；
	 * 也就是不作为物理刚体碰撞体使用。
	 * 对 Block/Overlap 的关系：QueryOnly 只是允许“查询系统”使用它；至于最终是 Block 还是 Overlap，仍由 CollisionResponse（通道响应设置）决定。
	 * 但它不会进入物理解算那条路径。
	 * 常见用途：投射物用碰撞体只做命中检测（Overlap/Hit 的 Query 路径），运动由 ProjectileMovementComponent 或自己更新位置，
	 * 不想让物理系统把它弹飞或影响其它物体。
	 */
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// 先忽略所有碰撞通道，然后再添加需要的重叠检测通道。
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	SphereCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);	
	// 创建投射物移动组件。
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	// 关闭自动激活，从对象池中取出时再手动激活。
	ProjectileMovement->bAutoActivate = false;
	/* 
	 * 告诉 ProjectileMovement 用哪个组件来移动/扫掠，有些情况下（尤其是池化反复 Deactivate/Activate），
	 * UpdatedComponent 为空会导致“看起来激活了但完全不动”。这行属于“强保险”。
	 */
	ProjectileMovement->SetUpdatedComponent(SphereCollision);	
	// 投射物移动组件的初始速度。
	ProjectileMovement->InitialSpeed = 550.0f;
	// 投射物移动组件的最大速度。
	ProjectileMovement->MaxSpeed = 550.0f;
	// 投射物移动组件的重力缩放系数，0表示忽略重力。。
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	// 创建对象池组件。
	PoolComponent = CreateDefaultSubobject<UPoolableComponent>("PoolComponent");	
}

void AScProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	// 池化对象不使用设置投射物的寿命函数，避免Bug，如果对象池有自动清理机制的话，需使用对象池子系统内的函数。
	//SetLifeSpan(LifeSpan);
	// 将球体碰撞重叠检测的回调绑定到开始重叠事件上。
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AScProjectileActor::OnSphereOverlap);
	// 使用 SpawnSoundAttached 把循环音效附加到根组件并播放音效，然后缓存，用于检测到碰撞时停止播放。
	LoopingSoundComp = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

/*void AScProjectileActor::Destroyed()
{
	// 投射物销毁时，如果客户端未生成特效和播放音效，在调用父类前调用一次处理命中效果的函数。
	if (!bHit && !HasAuthority())
	{
		// 生成特效。
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		// 播放声音。
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		// 检测到碰撞时停止播放循环音效。
		LoopingSoundComp->Stop();
	}
	// 池化对象不使用销毁。
	//UPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UPoolSubsystem>();
	//PoolSubsystem->ReleaseActor(this);
	Super::Destroyed();
}*/

void AScProjectileActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 生成特效。
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	// 播放声音。
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	// 检测到碰撞时停止播放循环音效，对象池组件中已经停止了音效，此处不再重复操作，如果有其他Bug的话可以考虑在这里手动操作。
	//LoopingSoundComp->Stop();
	
	// 池化对象不要使用 Destroy 销毁，使用 ReleaseToPool 归还。
	if (UPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UPoolSubsystem>())
	{PoolSubsystem->ReleaseToPool(this);}
	
	/* 
	 * 网络同步相关：池化对象暂时不做网络同步，有很多Bug暂时无法解决，此处仅保留课程示例代码。
	 * 如果在服务器上，则销毁Actor。
	if (HasAuthority())
	{Destroy();}
	// 如果不在服务器上，将bHit标记设为true，用于表示已经生成了特效和播放了音效。
	else
	{bHit = true;}
	*/
}