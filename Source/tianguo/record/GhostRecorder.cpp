// Copyright Epic Games, Inc. All Rights Reserved.

#include "GhostRecorder.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "tianguoCharacter.h"
#include "Camera/CameraComponent.h"

UGhostRecorder::UGhostRecorder()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UGhostRecorder::BeginPlay()
{
	Super::BeginPlay();
	ClearFrames();
}

void UGhostRecorder::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bRecording)
	{
		return;
	}

	RecordingTime += DeltaTime;

	// Update current frame timestamp
	CurrentFrame.TimeStamp = RecordingTime;

	// Record position and rotation of owner character
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		CurrentFrame.Location = OwnerCharacter->GetActorLocation();
		CurrentFrame.Rotation = OwnerCharacter->GetActorRotation();
		// Record camera rotation if available
		if (AtianguoCharacter* TianguoCharacter = Cast<AtianguoCharacter>(OwnerCharacter))
		{
			if (UCameraComponent* Camera = TianguoCharacter->GetFirstPersonCameraComponent())
			{
				CurrentFrame.CameraRotation = Camera->GetComponentRotation();
			}
		}
	}

	// Push frame if we have any data (movement, look, or actions)
	if (!CurrentFrame.MoveInput.IsZero() || !CurrentFrame.LookInput.IsZero() ||
		CurrentFrame.PressedActions != 0 || CurrentFrame.ReleasedActions != 0)
	{
		PushFrame();
	}

	// Check if recording duration has been reached
	if (RecordingTime >= RecordingDuration)
	{
		StopRecording();
	}
}

void UGhostRecorder::StartRecording(float Duration)
{
	if (bRecording)
	{
		return;
	}

	bRecording = true;
	RecordingDuration = Duration;
	RecordingTime = 0.0f;
	ClearFrames();

	// Initialize first frame
	CurrentFrame = FGhostFrame();
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		CurrentFrame.Location = OwnerCharacter->GetActorLocation();
		CurrentFrame.Rotation = OwnerCharacter->GetActorRotation();
		// Record camera rotation if available
		if (AtianguoCharacter* TianguoCharacter = Cast<AtianguoCharacter>(OwnerCharacter))
		{
			if (UCameraComponent* Camera = TianguoCharacter->GetFirstPersonCameraComponent())
			{
				CurrentFrame.CameraRotation = Camera->GetComponentRotation();
			}
		}
	}
	CurrentFrame.TimeStamp = 0.0f;
}

void UGhostRecorder::StopRecording()
{
	GEngine->AddOnScreenDebugMessage(
-1,        // 序号，-1不覆盖
1.f,       // 显示5秒
FColor::Green,
TEXT("结束录制")
);
	if (!bRecording)
	{
		return;
	}

	bRecording = false;
	// Push the last frame if it has any data
	if (!CurrentFrame.MoveInput.IsZero() || !CurrentFrame.LookInput.IsZero() ||
		CurrentFrame.PressedActions != 0 || CurrentFrame.ReleasedActions != 0)
	{
		PushFrame();
	}
}

void UGhostRecorder::ClearFrames()
{
	Frames.Empty();
	CurrentFrame = FGhostFrame();
}

void UGhostRecorder::RecordMoveInput(float Right, float Forward)
{
	if (!bRecording)
	{
		return;
	}
	CurrentFrame.MoveInput.X = Right;
	CurrentFrame.MoveInput.Y = Forward;
}

void UGhostRecorder::RecordLookInput(float Yaw, float Pitch)
{
	if (!bRecording)
	{
		return;
	}
	CurrentFrame.LookInput.X = Yaw;
	CurrentFrame.LookInput.Y = Pitch;
}

void UGhostRecorder::RecordActionPressed(EGhostAction Action)
{
	if (!bRecording)
	{
		return;
	}
	CurrentFrame.SetActionPressed(Action);
}

void UGhostRecorder::RecordActionReleased(EGhostAction Action)
{
	if (!bRecording)
	{
		return;
	}
	CurrentFrame.SetActionReleased(Action);
}

void UGhostRecorder::PushFrame()
{
	// Copy current frame and add to array
	Frames.Add(CurrentFrame);

	// Reset inputs and actions for next frame, keep timestamp, location, rotation, camera rotation
	FGhostFrame NewFrame;
	NewFrame.TimeStamp = CurrentFrame.TimeStamp;
	NewFrame.Location = CurrentFrame.Location;
	NewFrame.Rotation = CurrentFrame.Rotation;
	NewFrame.CameraRotation = CurrentFrame.CameraRotation;
	NewFrame.HeldActions = CurrentFrame.HeldActions; // Carry over held actions
	CurrentFrame = NewFrame;
}