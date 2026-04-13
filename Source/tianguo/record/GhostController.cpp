// Copyright Epic Games, Inc. All Rights Reserved.

#include "GhostController.h"
#include "Engine/Engine.h"

AGhostController::AGhostController()
{
	// This controller is not a player controller
	bIsPlayerController = false;
	// Don't possess a player
	// SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);
	
}

//void AGhostController::UpdateRotation(float DeltaTime)
//{
	// Do nothing – let the movement component handle rotation based on input
	// This prevents the controller from overriding the ghost's rotation
//}



//void AGhostController::ProcessPlayerInput(const float DeltaTime, const bool bGamePaused)
//{
	// Do not process any player input – ghost input comes from recorded frames
	// This prevents the controller from consuming input events
//}