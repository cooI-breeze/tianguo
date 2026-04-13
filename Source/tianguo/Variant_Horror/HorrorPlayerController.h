// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HorrorPlayerController.generated.h"

class UInputMappingContext;
class UHorrorUI;
class UInputAction;

/**
 *  Player Controller for a first person horror game
 *  Manages input mappings
 *  Manages UI
 */
UCLASS(abstract, config="Game")
class TIANGUO_API AHorrorPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Type of UI widget to spawn */
	UPROPERTY(EditAnywhere, Category="Horror|UI")
	TSubclassOf<UHorrorUI> HorrorUIClass;

	/** Pointer to the UI widget */
	UPROPERTY()
	TObjectPtr<UHorrorUI> HorrorUI;

public:

	/** Constructor */
	AHorrorPlayerController();

protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** Input action for starting recording */
	UPROPERTY(EditAnywhere, Category = "Input|Ghost")
	UInputAction* RecordAction;

	/** Input action for spawning a ghost */
	UPROPERTY(EditAnywhere, Category = "Input|Ghost")
	UInputAction* ReplayAction;

	/** Ghost blueprint class to spawn (set in blueprint instance) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Ghost")
	TSubclassOf<class AHorrorGhostCharacter> GhostBlueprintClass;
	
	/** Duration (in seconds) for ghost recording */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Ghost", meta = (ClampMin = "0.1", ClampMax = "60.0"))
	float RecordDuration = 3.0f;

	/** Gameplay Initialization */
	virtual void BeginPlay() override;

	/** Possessed pawn initialization */
	virtual void OnPossess(APawn* aPawn) override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Start recording ghost input */
	void OnStartRecord();

	/** Spawn a ghost at current location */
	void OnSpawnGhost();

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;
};
