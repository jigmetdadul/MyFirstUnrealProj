// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine/World.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/PlayerController.h"
#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"

#define OUT
// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	InitialYaw = GetOwner() -> GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw;

	FindpressurePlate();
	FindAudioComponent();
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner() -> FindComponentByClass<UAudioComponent>();

	if(!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s misssing audio component"), *GetOwner()-> GetName());
	}
}
void UOpenDoor::FindpressurePlate()
{
	if(!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s ActorName has the open door component, but no pressure plate!!!"), *GetOwner() -> GetName())
	}
}
// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(TotalMassOfActors() > MassToOpenDoors)
	{
      OpenDoor(DeltaTime);
	  DoorLastOpened = GetWorld() -> GetTimeSeconds();
	}
	else
	{
		if(GetWorld() -> GetTimeSeconds() - DoorLastOpened > DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
		}
	}
}
void UOpenDoor::OpenDoor(float DeltaTime)
{
    //UE_LOG(LogTemp, Warning, TEXT("%s"), *GetOwner() -> GetActorRotation().ToString());
	// UE_LOG(LogTemp, Warning, TEXT("Yaw is: %f"), GetOwner() -> GetActorRotation().Yaw);
	CurrentYaw = FMath::Lerp(CurrentYaw, OpenAngle, DeltaTime * DoorOpenSpeed);
	FRotator DoorRotation = GetOwner() -> GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner() -> SetActorRotation(DoorRotation);
    
	CloseDoorSound = false;
    if(!AudioComponent){return;}
    if(!OpenDoorSound)
	{
	AudioComponent -> Play();
	OpenDoorSound = true;
	}
}
void UOpenDoor::CloseDoor(float DeltaTime)
{
    //UE_LOG(LogTemp, Warning, TEXT("%s"), *GetOwner() -> GetActorRotation().ToString());
	// UE_LOG(LogTemp, Warning, TEXT("Yaw is: %f"), GetOwner() -> GetActorRotation().Yaw);
	CurrentYaw = FMath::Lerp(CurrentYaw, InitialYaw, DeltaTime * DoorCloseSpeed);
	FRotator DoorRotation = GetOwner() -> GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner() -> SetActorRotation(DoorRotation);
     
	OpenDoorSound = false; 
	if(!AudioComponent){return;}
    if(!CloseDoorSound)
	{
	AudioComponent -> Play();
	CloseDoorSound = true;
	}
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;
	TArray<AActor*> OverlappingActors;
	if(!PressurePlate){return TotalMass;}
	PressurePlate -> GetOverlappingActors(OverlappingActors);

	for(AActor* Actor: OverlappingActors)
	{
	    TotalMass += Actor -> FindComponentByClass<UPrimitiveComponent>() -> GetMass();
	}

	return TotalMass;
}