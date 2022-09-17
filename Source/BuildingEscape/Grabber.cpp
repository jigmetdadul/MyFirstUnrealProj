// Fill out your copyright notice in the Description page of Project Settings.
#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Grabber.h"

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
	SetupInputComponent();	
}
	
void UGrabber::SetupInputComponent()
{
    InputComponent = GetOwner() -> FindComponentByClass<UInputComponent>();
	if(InputComponent)
	{
		InputComponent -> BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent -> BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}	

//Checking for Physics handle component
void UGrabber::FindPhysicsHandle()	
{
	
PhysicsHandle = GetOwner() -> FindComponentByClass<UPhysicsHandleComponent>();
	if(!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("No Physics handle component found on %s!"), *GetOwner() -> GetName());
	}
	 
}
void UGrabber::Grab()
{	 
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
    UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();

	if(ActorHit)
	{
		if(!PhysicsHandle){return;}
	 PhysicsHandle -> GrabComponentAtLocation
	 (
		ComponentToGrab,
		NAME_None,
		GetPlayersReach()
	 );
	}
}


void UGrabber::Release()
{
	if(!PhysicsHandle){return;}
	PhysicsHandle -> ReleaseComponent();
}
 

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	 if(!PhysicsHandle){return;}
	 if(PhysicsHandle -> GrabbedComponent)
	 {
		 PhysicsHandle -> SetTargetLocation(GetPlayersReach());
	 }

}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
    FHitResult Hit;

	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld() -> LineTraceSingleByObjectType(
		OUT Hit,
		GetPlayersWorldPos(),
		GetPlayersReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);
	
	AActor* ActorHit = Hit.GetActor();
	if(ActorHit)
	{ 
		UE_LOG(LogTemp, Warning, TEXT("Line trace has hit: %s"), *(ActorHit -> GetName()));
	}

	return Hit;
}

FVector UGrabber::GetPlayersWorldPos() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
   
    GetWorld() -> GetFirstPlayerController() -> GetPlayerViewPoint(
		OUT PlayerViewPointLocation, 
		OUT PlayerViewPointRotation
	);
    return PlayerViewPointLocation;
}

FVector UGrabber::GetPlayersReach() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
   
    GetWorld() -> GetFirstPlayerController() -> GetPlayerViewPoint(
		OUT PlayerViewPointLocation, 
		OUT PlayerViewPointRotation
	);

    return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
	 
}