// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Variant_Horror/HorrorCharacter.h"
#include "GhostTypes.h"
#include "HorrorGhostCharacter.generated.h"

/**
 * Ghost character that replays recorded player actions.
 * Spawned at the player's location and replays the recorded frames.
 */
UCLASS()
class TIANGUO_API AHorrorGhostCharacter : public AHorrorCharacter
{
	GENERATED_BODY()

public:
	AHorrorGhostCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
public:

	
	// 方法重写
	virtual void DoMove(float Right, float Forward) override;
	virtual void DoAim(float Yaw, float Pitch) override;

public:
	/** Initialize the ghost with recorded frames and start replay */
	UFUNCTION(BlueprintCallable, Category = "Ghost")
	void InitializeGhost(const TArray<FGhostFrame>& InFrames);

	/** Start replaying the recorded frames */
	UFUNCTION(BlueprintCallable, Category = "Ghost")
	void StartReplay();

	/** Stop replay */
	UFUNCTION(BlueprintCallable, Category = "Ghost")
	void StopReplay();

	/** Returns true if currently replaying */
	UFUNCTION(BlueprintPure, Category = "Ghost")
	bool IsReplaying() const { return bReplaying; }

private:
	/** Whether we are currently replaying */
	bool bReplaying = false;

	/** Recorded frames to replay */
	TArray<FGhostFrame> Frames;

	/** Current replay time */
	float ReplayTime = 0.0f;

	/** Index of the last processed frame */
	int32 CurrentFrameIndex = 0;

	/** Apply a single frame's inputs to this character */
	void ApplyFrame(const FGhostFrame& Frame);

	/** Find the frame index for the given time stamp (binary search) */
	int32 FindFrameIndex(float TimeStamp) const;
};