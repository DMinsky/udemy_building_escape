// Fill out your copyright notice in the Description page of Project Settings.

#include "DoorOpen.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"

// Sets default values for this component's properties
UDoorOpen::UDoorOpen()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UDoorOpen::BeginPlay()
{
	Super::BeginPlay();
	FindAudioComponent();
	CurrentYaw = InitialYaw = GetOwner()->GetActorRotation().Yaw;
	TargetYaw += CurrentYaw;
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has attacheck DoorOpen component but PressurePlate is not set"), *GetOwner()->GetName());
	}
}


// Called every frame
void UDoorOpen::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GetTotalOverlapedMass() >= MassToOpenDoor)
	{
		OpenDoor(DeltaTime);
		LastTimeDoorOpen = GetWorld()->GetTimeSeconds();
	}
	else
	{
		if (GetWorld()->GetTimeSeconds() - LastTimeDoorOpen > DelayBeforeDoorClose) {
			CloseDoor(DeltaTime);
		}
	}
}

void UDoorOpen::CloseDoor(float DeltaTime)
{
	CurrentYaw = FMath::FInterpTo(CurrentYaw, InitialYaw, DeltaTime, DoorCloseSpeed);
	FRotator OpenDoorRotation = GetOwner()->GetActorRotation();
	OpenDoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(OpenDoorRotation);
	if (AudioComponent && LastDoorState == DoorState::Open)
	{
		AudioComponent->Play();
	}
	LastDoorState = DoorState::Close;
}

void UDoorOpen::OpenDoor(float DeltaTime)
{
	CurrentYaw = FMath::FInterpTo(CurrentYaw, TargetYaw, DeltaTime, DoorCloseSpeed);
	FRotator OpenDoorRotation = GetOwner()->GetActorRotation();
	OpenDoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(OpenDoorRotation);
	if (AudioComponent && LastDoorState == DoorState::Close)
	{
		AudioComponent->Play();
	}
	LastDoorState = DoorState::Open;
}

float UDoorOpen::GetTotalOverlapedMass() const
{
	float TotalMass = 0.0f;
	TArray<AActor*> OverlapingActors;
	if (!PressurePlate) { return TotalMass; } 	
	PressurePlate->GetOverlappingActors(OUT OverlapingActors);
	for (size_t i = 0; i < OverlapingActors.Num(); i++)
	{
		UPrimitiveComponent* primitive = OverlapingActors[i]->FindComponentByClass<UPrimitiveComponent>();
		if (primitive)
		{
			TotalMass += primitive->GetMass();
		}
	}
	// UE_LOG(LogTemp, Warning, TEXT("Total mass: %f"), TotalMass);
	return TotalMass;
}

void UDoorOpen::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	if (!AudioComponent) {
		UE_LOG(LogTemp, Error, TEXT("%s has no UAudioComponent!"), *(GetOwner()->GetName()));
	}
}