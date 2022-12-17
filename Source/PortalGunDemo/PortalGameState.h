// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PortalWall.h"
#include "PortalGameState.generated.h"

/**
 * 
 */
UCLASS()
class PORTALGUNDEMO_API APortalGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	bool IsPortalADown() { return PortalADown; }
	bool IsPortalBDown() { return PortalBDown; }

	void PutPortalADown() { PortalADown = true; }
	void PutPortalBDown() { PortalBDown = true; }

	void PickPortalAUp() { PortalADown = false; }
	void PickPortalBUp() { PortalBDown = false; }

	APortalWall* GetActivePortalA() { return ActivePortalA; }
	APortalWall* GetActivePortalB() { return ActivePortalB; }

	void SetActivePortalA(APortalWall* NewPortal) { ActivePortalA = NewPortal; }
	void SetActivePortalB(APortalWall* NewPortal) { ActivePortalB = NewPortal; }

	void DelActivePortalA() { ActivePortalA = nullptr; }
	void DelActivePortalB() { ActivePortalB = nullptr; }

	bool IsPortalATeleporting() { return PortalATeleporting; }
	bool IsPortalBTeleporting() { return PortalBTeleporting; }

	void SetPortalATeleporting() { PortalATeleporting = true; }
	void SetPortalBTeleporting() { PortalBTeleporting = true; }

	void PortalADoneTeleporting() { PortalATeleporting = false; }
	void PortalBDoneTeleporting() { PortalBTeleporting = false; }

private:
	bool PortalADown;
	bool PortalBDown;

	bool PortalATeleporting;
	bool PortalBTeleporting;

	APortalWall* ActivePortalA;
	APortalWall* ActivePortalB;

};
