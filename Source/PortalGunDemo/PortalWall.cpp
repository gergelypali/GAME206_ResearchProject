// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalWall.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PortalGunDemoCharacter.h"
#include "PortalGameState.h"
#include "Components/ArrowComponent.h"
#include "PortalGunDemoCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PortalGunDemoProjectile.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APortalWall::APortalWall()
{
	Tags.Add(FName("PortalWall"));
	RootComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	
	UArrowComponent* Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DirectionArrow"));
	Arrow->SetHiddenInGame(false);
	Arrow->SetupAttachment(RootComponent);

	PortalWallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalWallMesh"));
	PortalWallMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (MeshAsset.Succeeded())
	{
		PortalWallMesh->SetStaticMesh(MeshAsset.Object);
	}
	PortalWallMesh->SetRelativeScale3D(FVector(0.1f, 1.0f, 2.5f));

	PortalWallCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PortalWallCollisionBox"));
	PortalWallCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APortalWall::OnOverlapBegin);
	PortalWallCollisionBox->InitBoxExtent(FVector(7, 50, 125));
	PortalWallCollisionBox->SetCollisionProfileName("BlockAll");
	PortalWallCollisionBox->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UMaterial> InactiveMaterialAsset = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/M_PortalInactive.M_PortalInactive'"));
	if (InactiveMaterialAsset.Succeeded())
	{
		InactiveMaterial = InactiveMaterialAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> ActiveAMaterialAsset = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/M_PortalAActive.M_PortalAActive'"));
	if (ActiveAMaterialAsset.Succeeded())
	{
		ActiveAMaterial = ActiveAMaterialAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> ActiveBMaterialAsset = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/M_PortalBActive.M_PortalBActive'"));
	if (ActiveBMaterialAsset.Succeeded())
	{
		ActiveBMaterial = ActiveBMaterialAsset.Object;
	}
	PortalWallMesh->SetMaterial(0, InactiveMaterial);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APortalWall::BeginPlay()
{
	Super::BeginPlay();
	APortalGunDemoCharacter* PlayerChar = Cast<APortalGunDemoCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	PlayerChar->OnResetWall.AddDynamic(this, &APortalWall::ResetWall);
	PortalWallCollisionBox->OnComponentEndOverlap.AddDynamic(this, &APortalWall::OnOverlapEnd);
	GS = Cast<APortalGameState>(GetWorld()->GetGameState());

}

// Called every frame
void APortalWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APortalWall::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GS->GetActivePortalA() == this && GS->IsPortalBTeleporting())
	{
		GS->PortalBDoneTeleporting();
	}
	if (GS->GetActivePortalB() == this && GS->IsPortalATeleporting())
	{
		GS->PortalADoneTeleporting();
	}
}

void APortalWall::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName("PortalA")))
	{
		OtherActor->Destroy();
		if (!GS->IsPortalADown() && GS->GetActivePortalB() != this)
		{
			PortalWallMesh->SetMaterial(0, ActiveAMaterial);
			GS->PutPortalADown();
			GS->SetActivePortalA(this);
		}
	}
	else if (OtherActor->ActorHasTag(FName("PortalB")))
	{
		OtherActor->Destroy();
		if (!GS->IsPortalBDown() && GS->GetActivePortalA() != this)
		{
			PortalWallMesh->SetMaterial(0, ActiveBMaterial);
			GS->PutPortalBDown();
			GS->SetActivePortalB(this);
		}
	}
	if (GS->IsPortalADown() && GS->IsPortalBDown())
	{
		if (OtherActor->ActorHasTag(FName("Test")) || OtherActor->ActorHasTag(FName("Player")))
		{
			if (GS->GetActivePortalA() == this && !GS->IsPortalBTeleporting())
			{
				GS->SetPortalATeleporting();
				// calculate the rotator between the two portals from PortalA perspective
				FQuat quatF = FQuat::FindBetween(GS->GetActivePortalA()->GetActorForwardVector() * -1, GS->GetActivePortalB()->GetActorForwardVector());
				FQuat quatR = FQuat::FindBetween(GS->GetActivePortalA()->GetActorRightVector() * -1, GS->GetActivePortalB()->GetActorRightVector());

				if (OtherActor->ActorHasTag(FName("Player")))
				{
					OtherActor->SetActorLocation(GS->GetActivePortalB()->GetActorLocation());
					Cast<APortalGunDemoCharacter>(OtherActor)->GetCharacterMovement()->Velocity = quatF.RotateVector(Cast<APortalGunDemoCharacter>(OtherActor)->GetCharacterMovement()->Velocity);
					Cast<APortalGunDemoCharacter>(OtherActor)->GetLocalViewingPlayerController()->SetControlRotation(FRotator(quatR));
				}
				if (OtherActor->ActorHasTag(FName("Test")))
				{
					OtherActor->SetActorLocation(GS->GetActivePortalB()->GetActorLocation());
					Cast<APortalGunDemoProjectile>(OtherActor)->GetProjectileMovement()->Velocity = FRotator(quatF).RotateVector(Cast<APortalGunDemoProjectile>(OtherActor)->GetProjectileMovement()->Velocity);
				}
			}
			if (GS->GetActivePortalB() == this && !GS->IsPortalATeleporting())
			{
				GS->SetPortalBTeleporting();
				// calculate the rotator between the two portals from PortalA perspective
				FQuat quatF = FQuat::FindBetween(GS->GetActivePortalB()->GetActorForwardVector() * -1, GS->GetActivePortalA()->GetActorForwardVector());
				FQuat quatR = FQuat::FindBetween(GS->GetActivePortalB()->GetActorRightVector() * -1, GS->GetActivePortalA()->GetActorRightVector());

				if (OtherActor->ActorHasTag(FName("Player")))
				{
					OtherActor->SetActorLocation(GS->GetActivePortalA()->GetActorLocation());
					Cast<APortalGunDemoCharacter>(OtherActor)->GetCharacterMovement()->Velocity = quatF.RotateVector(Cast<APortalGunDemoCharacter>(OtherActor)->GetCharacterMovement()->Velocity);
					Cast<APortalGunDemoCharacter>(OtherActor)->GetLocalViewingPlayerController()->SetControlRotation(FRotator(quatR));
				}
				if (OtherActor->ActorHasTag(FName("Test")))
				{
					OtherActor->SetActorLocation(GS->GetActivePortalA()->GetActorLocation());
					Cast<APortalGunDemoProjectile>(OtherActor)->GetProjectileMovement()->Velocity = FRotator(quatF).RotateVector(Cast<APortalGunDemoProjectile>(OtherActor)->GetProjectileMovement()->Velocity);
				}

			}
		}
	}
}

void APortalWall::ResetWall()
{
	PortalWallMesh->SetMaterial(0, InactiveMaterial);
	GS->PickPortalAUp();
	GS->DelActivePortalA();
	GS->PickPortalBUp();
	GS->DelActivePortalB();
}
