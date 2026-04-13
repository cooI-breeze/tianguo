// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GhostTypes.h"
#include "GhostRecorder.generated.h"

/**
 * Component that records player inputs and transforms into a series of frames.
 * Attach to a character to enable recording.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TIANGUO_API UGhostRecorder : public UActorComponent
{
	GENERATED_BODY()

public:
	UGhostRecorder();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/** Start recording for the specified duration (seconds) */
	UFUNCTION(BlueprintCallable, Category = "Ghost")
	void StartRecording(float Duration);

	/** Stop recording early */
	UFUNCTION(BlueprintCallable, Category = "Ghost")
	void StopRecording();

	/** Returns true if currently recording */
	UFUNCTION(BlueprintPure, Category = "Ghost")
	bool IsRecording() const { return bRecording; }

	/** Get the recorded frames (const) */
	const TArray<FGhostFrame>& GetFrames() const { return Frames; }

	/** Clear recorded frames */
	UFUNCTION(BlueprintCallable, Category = "Ghost")
	void ClearFrames();

	// Recording hooks (called by character)
	void RecordMoveInput(float Right, float Forward);
	void RecordLookInput(float Yaw, float Pitch);
	void RecordActionPressed(EGhostAction Action);
	void RecordActionReleased(EGhostAction Action);

private:
	/** Whether we are currently recording */
	bool bRecording = false;

	/** How long to record (seconds) */
	float RecordingDuration = 0.0f;

	/** Current recording time */
	float RecordingTime = 0.0f;

	/** Recorded frames */
	TArray<FGhostFrame> Frames;

	/** Current frame being built */
	FGhostFrame CurrentFrame;

	/** Finalize and store the current frame, start a new one */
	void PushFrame();
};