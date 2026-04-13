// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Horror/HorrorPlayerController.h"
#include "record/GhostRecorder.h"
#include "record/HorrorGhostCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "tianguoCameraManager.h"
#include "HorrorCharacter.h"
#include "EnhancedInputComponent.h"
#include "HorrorUI.h"
#include "tianguo.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "InputAction.h"

AHorrorPlayerController::AHorrorPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AtianguoCameraManager::StaticClass();
}

void AHorrorPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(Logtianguo, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void AHorrorPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	// only spawn UI on local player controllers
	if (IsLocalPlayerController())
	{
		// set up the UI for the character
		if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(aPawn))
		{
			// create the UI
			if (!HorrorUI)
			{
				HorrorUI = CreateWidget<UHorrorUI>(this, HorrorUIClass);
				HorrorUI->AddToViewport(0);
			}

			HorrorUI->SetupCharacter(HorrorCharacter);
		}
	}
	
}

void AHorrorPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}

	// Bind ghost recording/replay actions
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (RecordAction)
		{
			EnhancedInputComponent->BindAction(RecordAction, ETriggerEvent::Started, this, &AHorrorPlayerController::OnStartRecord);
		}
		if (ReplayAction)
		{
			EnhancedInputComponent->BindAction(ReplayAction, ETriggerEvent::Started, this, &AHorrorPlayerController::OnSpawnGhost);
		}
	}
}

bool AHorrorPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void AHorrorPlayerController::OnStartRecord()
{
	GEngine->AddOnScreenDebugMessage(
	-1,        // 序号，-1不覆盖
	1.f,       // 显示5秒
	FColor::Green,
	TEXT("开始录制")
);
	if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(GetPawn()))
	{
		if (UGhostRecorder* Recorder = HorrorCharacter->GetGhostRecorder())
		{
			Recorder->StartRecording(RecordDuration); // record 3 seconds
		}
	}
}

void AHorrorPlayerController::OnSpawnGhost()
{	GEngine->AddOnScreenDebugMessage(
	-1,        // 序号，-1不覆盖
	1.f,       // 显示5秒
	FColor::Green,
	TEXT("开始回放")
);
	if (!GetPawn())
	{
		return;
	}

	// Get recorded frames from player's ghost recorder
	AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(GetPawn());
	UGhostRecorder* Recorder = HorrorCharacter ? HorrorCharacter->GetGhostRecorder() : nullptr;
	if (!Recorder)
	{
		return;
	}

	const TArray<FGhostFrame>& Frames = Recorder->GetFrames();
	if (Frames.Num() == 0)
	{
		return; // nothing recorded
	}

	UWorld* World = GetWorld();

	// Spawn ghost at player's current location
	FTransform SpawnTransform = HorrorCharacter->GetActorTransform();
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	
	// Determine which class to spawn (use configured blueprint class if set)
	UClass* GhostClassToSpawn =    GhostBlueprintClass;
	
	// 使用蓝图类生成幽灵
	AHorrorGhostCharacter* Ghost = World->SpawnActor<AHorrorGhostCharacter>(
		GhostClassToSpawn,  // ← 使用蓝图类！
		SpawnTransform, SpawnParams
	);
	
		Ghost->InitializeGhost(Frames);
		Ghost->StartReplay();
}
