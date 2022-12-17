// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalWall.generated.h"

UCLASS()
class PORTALGUNDEMO_API APortalWall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalWall();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* PortalWallMesh;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* PortalWallCollisionBox;

	UFUNCTION()
	void ResetWall();

	class APortalGameState* GS;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class UMaterialInterface* InactiveMaterial;
	class UMaterialInterface* ActiveAMaterial;
	class UMaterialInterface* ActiveBMaterial;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
