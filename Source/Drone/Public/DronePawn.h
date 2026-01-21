// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DronePawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UBoxComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class DRONE_API ADronePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADronePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DroneMesh;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* FlyUpAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void FlyUp(const FInputActionValue& Value);

private:
	UPROPERTY(EditAnywhere, Category = "Drone|Stats")
	float MoveSpeed = 600.0f;
	UPROPERTY(EditAnywhere, Category = "Drone|Stats")
	float RotationSpeed = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Drone|Stats")
	float GravityAcceleration = -980.0f;
	float VerticalVelocity = 0.0f; // 수직 속도(낙하 속도)
	UPROPERTY(EditAnywhere, Category = "Drone|Stats")
	float ThrusterPower = 2000.0f; // 상승 가속도

};