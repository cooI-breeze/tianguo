// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Horror/HorrorCharacter.h"
#include "record/GhostRecorder.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"

AHorrorCharacter::AHorrorCharacter()
{
	// create the spotlight
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(GetFirstPersonCameraComponent());

	SpotLight->SetRelativeLocationAndRotation(FVector(30.0f, 17.5f, -5.0f), FRotator(-18.6f, -1.3f, 5.26f));
	SpotLight->Intensity = 0.5;
	SpotLight->SetIntensityUnits(ELightUnits::Lumens);
	SpotLight->AttenuationRadius = 1050.0f;
	SpotLight->InnerConeAngle = 18.7f;
	SpotLight->OuterConeAngle = 45.24f;

	// create ghost recorder component
	GhostRecorder = CreateDefaultSubobject<UGhostRecorder>(TEXT("GhostRecorder"));
}

void AHorrorCharacter::BeginPlay()
{
	Super::BeginPlay();

	// initialize sprint meter to max
	SprintMeter = SprintTime;

	// Initialize the walk speed
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// start the sprint tick timer
	GetWorld()->GetTimerManager().SetTimer(SprintTimer, this, &AHorrorCharacter::SprintFixedTick, SprintFixedTickTime, true);
}

void AHorrorCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// clear the sprint timer
	GetWorld()->GetTimerManager().ClearTimer(SprintTimer);
}

void AHorrorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

		{
			// Set up action bindings
			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
			{
				// Sprinting
				EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoStartSprint);
				EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AHorrorCharacter::DoEndSprint);

				// Interact
				EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoInteract);

			}
		}
	}

void AHorrorCharacter::DoStartSprint()
{
	// record sprint press
	if (GhostRecorder)
	{
		GhostRecorder->RecordActionPressed(EGhostAction::SprintPress);
	}

	// set the sprinting flag
	bSprinting = true;

	// are we out of recovery mode?
	if (!bRecovering)
	{
		// set the sprint walk speed
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

		// call the sprint state changed delegate
		OnSprintStateChanged.Broadcast(true);
	}
}

void AHorrorCharacter::DoEndSprint()
{
	// record sprint release
	if (GhostRecorder)
	{
		GhostRecorder->RecordActionReleased(EGhostAction::SprintRelease);
	}

	// set the sprinting flag
	bSprinting = false;

	// are we out of recovery mode?
	if (!bRecovering)
	{
		// set the default walk speed
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

		// call the sprint state changed delegate
		OnSprintStateChanged.Broadcast(false);
	}
}

void AHorrorCharacter::SprintFixedTick()
{
	// are we out of recovery, still have stamina and are moving faster than our walk speed?
	if (bSprinting && !bRecovering && GetVelocity().Length() > WalkSpeed)
	{

		// do we still have meter to burn?
		if (SprintMeter > 0.0f)
		{
			// update the sprint meter
			SprintMeter = FMath::Max(SprintMeter - SprintFixedTickTime, 0.0f);

			// have we run out of stamina?
			if (SprintMeter <= 0.0f)
			{
				// raise the recovering flag
				bRecovering = true;

				// set the recovering walk speed
				GetCharacterMovement()->MaxWalkSpeed = RecoveringWalkSpeed;
			}
		}
		
	} else {

		// recover stamina
		SprintMeter = FMath::Min(SprintMeter + SprintFixedTickTime, SprintTime);

		if (SprintMeter >= SprintTime)
		{
			// lower the recovering flag
			bRecovering = false;

			// set the walk or sprint speed depending on whether the sprint button is down
			GetCharacterMovement()->MaxWalkSpeed = bSprinting ? SprintSpeed : WalkSpeed;

			// update the sprint state depending on whether the button is down or not
			OnSprintStateChanged.Broadcast(bSprinting);
		}

	}

	// broadcast the sprint meter updated delegate
	OnSprintMeterUpdated.Broadcast(SprintMeter / SprintTime);

}

AActor* AHorrorCharacter::GetLookedAtInteractable()
{
	FVector Start;
	FRotator CameraRotation;
    
	// 优先使用第一人称摄像机组件
	if (UCameraComponent* Camera = GetFirstPersonCameraComponent())
	{
		Start = Camera->GetComponentLocation();
		CameraRotation = Camera->GetComponentRotation();
      FRotator  Rotation1114514=Camera->GetRelativeRotation();
	}
    
	FVector End = Start + CameraRotation.Vector() * 300.0f; 
    
	// 可视化调试线
	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f, 0, 1.0f);
    
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1, QueryParams))
	{
		return HitResult.GetActor();
	}
	return nullptr;
}

void AHorrorCharacter::DoInteract()
{
	// record interact action
	if (GhostRecorder)
	{
		GhostRecorder->RecordActionPressed(EGhostAction::Interact);
	}

	AActor* HitActor = GetLookedAtInteractable();
	if (HitActor)
	{
		// Blueprint可在此事件中处理交互
		BP_OnInteract(HitActor);
	}
}

void AHorrorCharacter::DoMove(float Right, float Forward)
{
	// record movement input
	if (GhostRecorder)
	{
		GhostRecorder->RecordMoveInput(Right, Forward);
	}

	// call parent implementation
	Super::DoMove(Right, Forward);
}
void AHorrorCharacter::DoAim(float Yaw, float Pitch)
{
	// record look input
	if (GhostRecorder)
	{
		GhostRecorder->RecordLookInput(Yaw, Pitch);
	}
	Super::DoAim(Yaw, Pitch);
}
