#include "DronePawn.h"
#include "GameFramework\SpringArmComponent.h"
#include "Camera\CameraComponent.h"
#include "Components\BoxComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
ADronePawn::ADronePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxCollision);

	BoxCollision->SetBoxExtent(FVector(40.f, 40.f, 15.f));
	BoxCollision->SetSimulatePhysics(false);
	BoxCollision->SetCollisionProfileName(TEXT("Pawn"));

	DroneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DroneMesh"));
	DroneMesh->SetupAttachment(RootComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 250.0f;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
}

void ADronePawn::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("DefaultMappingContext is NULL!"));
			}
		}
	}
}

// Called every frame
void ADronePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 중력 적용 (v = v0 + at)
	VerticalVelocity += GravityAcceleration * DeltaTime;

	// 최종 이동량 계산 (d = vt)
	// 중력은 월드 좌표계의 Z축으로 작용하므로 FVector(0,0,Z).
	FVector DropDelta = FVector(0.f, 0.f, VerticalVelocity * DeltaTime);

	// 충돌 감지 (Sweep = true)
	FHitResult HitResult;
	AddActorWorldOffset(DropDelta, true, &HitResult);

	// 바닥 충돌 처리
	if (HitResult.IsValidBlockingHit())
	{
		// 바닥에 부딪혔는데 속도가 아래를 향하고 있었다면? (착지)
		if (VerticalVelocity < 0.f)
		{
			VerticalVelocity = 0.f; // 속도 초기화
		}
	}

}

// Called to bind functionality to input
void ADronePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			UE_LOG(LogTemp, Warning, TEXT("MoveAction!"));
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADronePawn::Move);
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADronePawn::Look);
		}

		if (FlyUpAction)
		{
			EnhancedInputComponent->BindAction(FlyUpAction, ETriggerEvent::Triggered, this, &ADronePawn::FlyUp);
		}
	}

}

void ADronePawn::Move(const FInputActionValue& Value)
{
	FVector2D MoveVector = Value.Get<FVector2D>();
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	FVector DeltaLocation(
		MoveVector.X * MoveSpeed * DeltaTime,
		MoveVector.Y * MoveSpeed * DeltaTime,
		0.f
	);

	AddActorLocalOffset(DeltaLocation, true);
}

void ADronePawn::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	float YawDeg = LookVector.X * RotationSpeed * DeltaTime;
	float PitchDeg = -LookVector.Y * RotationSpeed * DeltaTime; // 마우스 반전 시 +

	AddActorLocalRotation(FRotator(PitchDeg, YawDeg, 0.f));
}

void ADronePawn::FlyUp(const FInputActionValue& Value)
{
	float FlyValue = Value.Get<float>();
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	//// Z: 상하 이동
	//FVector DeltaLocation(0.f, 0.f, FlyValue * MoveSpeed * DeltaTime);

	//AddActorLocalOffset(DeltaLocation, true);

	VerticalVelocity += FlyValue * ThrusterPower * DeltaTime;
}

