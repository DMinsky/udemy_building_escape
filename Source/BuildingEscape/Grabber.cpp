// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Grabber.h"
#include "DrawDebugHelpers.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandle();
	SetupInput();	
}

void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle) {
		UE_LOG(LogTemp, Error, TEXT("%s has no UPhysicsHandleComponent!"), *(GetOwner()->GetName()));
	}
}

void UGrabber::SetupInput()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) 
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Release", IE_Released, this, &UGrabber::Release);
	}
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Log, TEXT("GRAB"));
	FHitResult HitResult = GetFirstPhysicalBodyInReach();
	AActor* HitActor = HitResult.GetActor();	
	if (HitActor)
	{
		UE_LOG(LogTemp, Log, TEXT("GRAB %s"), *HitActor->GetName());
		UPrimitiveComponent* GrabComponent = HitResult.GetComponent();
		if (!PhysicsHandle) { return; }
		PhysicsHandle->GrabComponentAtLocation(
			GrabComponent,
			NAME_None,
			GetLookVector(Reach)
		);
	}
}

void UGrabber::Release()
{
	if (!PhysicsHandle) { return; }
	if (PhysicsHandle->GrabbedComponent) {
		PhysicsHandle->ReleaseComponent();
	}
}

void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!PhysicsHandle) { return; }
	if (PhysicsHandle->GrabbedComponent) {
		PhysicsHandle->SetTargetLocation(GetLookVector(Reach));
	}
}

FHitResult UGrabber::GetFirstPhysicalBodyInReach() const
{
	FHitResult HitResult;
	FCollisionQueryParams QueryParams(FName(TEXT("")), false, GetOwner());
	bool Hit = GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		GetPlayerLocation(),
		GetLookVector(Reach),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		QueryParams
	);
	return HitResult;
}

FVector UGrabber::GetLookVector(float Distance) const
{
	FVector PlayerLocation;
    FRotator PlayerRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerLocation, OUT PlayerRotation);
	return PlayerLocation + PlayerRotation.Vector() * Distance;
}

FVector UGrabber::GetPlayerLocation() const
{
	FVector PlayerLocation;
    FRotator PlayerRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerLocation, OUT PlayerRotation);
	return PlayerLocation;
}