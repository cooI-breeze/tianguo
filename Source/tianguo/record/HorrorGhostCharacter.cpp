// Copyright Epic Games, Inc. All Rights Reserved.

#include "HorrorGhostCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "GhostController.h"
#include "Engine/Engine.h"
#include "Camera/CameraComponent.h"

AHorrorGhostCharacter::AHorrorGhostCharacter()
{
	// Ghost should not be controlled by a player controller
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	// Use our custom ghost controller
	AIControllerClass = AGhostController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AHorrorGhostCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	
	// 关键配置：让移动组件像玩家一样处理输入
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bUseControllerDesiredRotation = true;  // 改为 true，允许控制器控制旋转
		MoveComp->bOrientRotationToMovement = false;
		MoveComp->bIgnoreBaseRotation = true;
		MoveComp->bConstrainToPlane = false;
		MoveComp->bSnapToPlaneAtStart = false;
		MoveComp->bForceMaxAccel = false;
		MoveComp->bRequestedMoveUseAcceleration = true;
		
	}// 禁用摄像机的 pawn 控制旋转，让我们自己控制
	if (UCameraComponent* Camera = GetFirstPersonCameraComponent())
	{
		Camera->bUsePawnControlRotation = false;
		UE_LOG(LogTemp, Warning, TEXT("[Ghost] 已禁用摄像机 bUsePawnControlRotation"));
	}
}

void AHorrorGhostCharacter::DoMove(float Right, float Forward)
{
	Super::DoMove(Right, Forward);// 调试4：调用后的速度
}

void AHorrorGhostCharacter::DoAim(float Yaw, float Pitch)
{
	Yaw*=2.5;
	Pitch*=-2.5;
	
	// 方案1：直接旋转整个角色（影响移动方向）
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += Yaw;  // 左右旋转角色身体
    
	// 对于第一人称，通常只旋转Yaw，Pitch通过摄像机单独处理
	SetActorRotation(NewRotation);
    
	// 2. 旋转摄像机（Pitch）
	if (UCameraComponent* Camera = GetFirstPersonCameraComponent())
	{
		// 获取当前世界旋转
		FRotator CurrentWorldRot = Camera->GetComponentRotation();
        
		// 直接在现有世界旋转上增加Pitch
		FRotator NewWorldRot = CurrentWorldRot;
		NewWorldRot.Pitch += Pitch;
		NewWorldRot.Pitch = FRotator::ClampAxis(NewWorldRot.Pitch);
        
		// 设置世界旋转
		Camera->SetWorldRotation(NewWorldRot);
        
	}
}


void AHorrorGhostCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bReplaying || Frames.Num() == 0)
	{
		return;
	}

	ReplayTime += DeltaTime;

	// Find the frame for the current time
	int32 FrameIndex = FindFrameIndex(ReplayTime);
	if (FrameIndex < 0 || FrameIndex >= Frames.Num()||FrameIndex==INDEX_NONE)
	{
		// Replay finished
		StopReplay();
		return;
	}

	// Apply all frames from CurrentFrameIndex up to FrameIndex
	while (CurrentFrameIndex <= FrameIndex && CurrentFrameIndex < Frames.Num())
	{
		ApplyFrame(Frames[CurrentFrameIndex]);
		CurrentFrameIndex++;
	}
}

void AHorrorGhostCharacter::InitializeGhost(const TArray<FGhostFrame>& InFrames)
{
	Frames = InFrames;
	SetActorLocationAndRotation(InFrames[0].Location, InFrames[0].Rotation);

	// Set camera rotation from recorded data
	if (UCameraComponent* Camera = GetFirstPersonCameraComponent())
	{
		Camera->SetWorldRotation(InFrames[0].CameraRotation);
	}

	ReplayTime = 0.0f;
	CurrentFrameIndex = 0;
	bReplaying = false;
}

void AHorrorGhostCharacter::StartReplay()
{
	if (bReplaying || Frames.Num() == 0)
	{
		return;
	}
	
	// 重置移动状态
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->Velocity = FVector::ZeroVector;
		MoveComp->StopMovementImmediately();
	}
	bReplaying = true;
	ReplayTime = 0.0f;
	CurrentFrameIndex = 0;
}

void AHorrorGhostCharacter::StopReplay()
{
	bReplaying = false;
}

void AHorrorGhostCharacter::ApplyFrame(const FGhostFrame& Frame)
{
	// Apply movement input
	DoMove(Frame.MoveInput.X, Frame.MoveInput.Y);

	// Apply look input
	DoAim(Frame.LookInput.X, Frame.LookInput.Y);

	// Apply actions
	if (Frame.IsActionPressed(EGhostAction::SprintPress))
	{
		DoStartSprint();
	}
	if (Frame.IsActionReleased(EGhostAction::SprintRelease))
	{
		DoEndSprint();
	}
	if (Frame.IsActionPressed(EGhostAction::Interact))
	{
		DoInteract();
	}
}

int32 AHorrorGhostCharacter::FindFrameIndex(float TimeStamp) const
{
	
	// 如果时间戳超过最后一帧，返回无效索引（触发停止回放）
	if (TimeStamp > Frames[Frames.Num()-1].TimeStamp) return INDEX_NONE;
	
	// Simple linear search (frames are few and recorded at high frequency)
	for (int32 i = 0; i < Frames.Num(); ++i)
	{
		if (Frames[i].TimeStamp >= TimeStamp)
		{
			return i;
		}
	}
	return Frames.Num() - 1;
}