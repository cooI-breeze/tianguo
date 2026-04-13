// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GhostTypes.generated.h"

UENUM(BlueprintType)
enum class EGhostAction : uint8
{
	SprintPress		UMETA(DisplayName = "Sprint Press"),
	SprintRelease	UMETA(DisplayName = "Sprint Release"),
	Interact		UMETA(DisplayName = "Interact"),
	MAX				UMETA(Hidden)
};

/** Single frame of ghost recording */
USTRUCT(BlueprintType)
struct FGhostFrame
{
	GENERATED_BODY()

public:
	/** Time stamp in seconds since recording started */
	UPROPERTY(BlueprintReadOnly, Category = "Ghost")
	float TimeStamp = 0.0f;

	/** Position of the character at this frame */
	UPROPERTY(BlueprintReadOnly, Category = "Ghost")
	FVector Location = FVector::ZeroVector;

	/** Rotation of the character at this frame */
	UPROPERTY(BlueprintReadOnly, Category = "Ghost")
	FRotator Rotation = FRotator::ZeroRotator;

	/** Camera rotation (world rotation) at this frame */
	UPROPERTY(BlueprintReadOnly, Category = "Ghost")
	FRotator CameraRotation = FRotator::ZeroRotator;

	/** Movement input vector (X = right, Y = forward) */
	UPROPERTY(BlueprintReadOnly, Category = "Ghost")
	FVector2D MoveInput = FVector2D::ZeroVector;

	/** Look input vector (X = yaw, Y = pitch) */
	UPROPERTY(BlueprintReadOnly, Category = "Ghost")
	FVector2D LookInput = FVector2D::ZeroVector;

	/** Actions that were pressed this frame (bitmask) */
	UPROPERTY(BlueprintReadOnly, Category = "Ghost")
	uint8 PressedActions = 0;

	/** Actions that were released this frame (bitmask) */
	UPROPERTY(BlueprintReadOnly, Category = "Ghost")
	uint8 ReleasedActions = 0;

	/** Actions that are being held this frame (bitmask) */
	UPROPERTY(BlueprintReadOnly, Category = "Ghost")
	uint8 HeldActions = 0;

	/** Helper to set an action as pressed */
	void SetActionPressed(EGhostAction Action)
	{
		const uint8 Bit = 1 << static_cast<uint8>(Action);
		PressedActions |= Bit;
		HeldActions |= Bit;
	}

	/** Helper to set an action as released */
	void SetActionReleased(EGhostAction Action)
	{
		const uint8 Bit = 1 << static_cast<uint8>(Action);
		ReleasedActions |= Bit;
		HeldActions &= ~Bit;
	}

	/** Check if an action is held */
	bool IsActionHeld(EGhostAction Action) const
	{
		const uint8 Bit = 1 << static_cast<uint8>(Action);
		return (HeldActions & Bit) != 0;
	}

	/** Check if an action was pressed this frame */
	bool IsActionPressed(EGhostAction Action) const
	{
		const uint8 Bit = 1 << static_cast<uint8>(Action);
		return (PressedActions & Bit) != 0;
	}

	/** Check if an action was released this frame */
	bool IsActionReleased(EGhostAction Action) const
	{
		const uint8 Bit = 1 << static_cast<uint8>(Action);
		return (ReleasedActions & Bit) != 0;
	}
};