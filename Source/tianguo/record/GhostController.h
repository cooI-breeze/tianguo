// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GhostController.generated.h"

/**
 * Simple controller for ghost characters that need to process movement input
 * without AI behavior or player input handling.
 */
UCLASS()
class TIANGUO_API AGhostController : public AAIController
{
	GENERATED_BODY()

public:
	AGhostController();

	//~ Begin AController Interface
	// virtual void UpdateRotation(float DeltaTime) override;
	// virtual void ProcessPlayerInput(const float DeltaTime, const bool bGamePaused) override;
	//~ End AController Interface
};