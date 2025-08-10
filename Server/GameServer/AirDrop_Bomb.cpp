#include "pch.h"
#include "AirDrop_Bomb.h"

AirDrop_Bomb::AirDrop_Bomb()
{
}

AirDrop_Bomb::~AirDrop_Bomb()
{
}

void AirDrop_Bomb::Initialize()
{
}

int AirDrop_Bomb::Update(float deltaTime)
{

	if (_isDead) {
		return 1;
	}

	ProcessMove(deltaTime);

	return 0;
}

void AirDrop_Bomb::Late_Update()
{
}

void AirDrop_Bomb::Release()
{
}

void AirDrop_Bomb::SetInitData(uint8 playerID, uint8 TankIndex ,Vec3 InitPos)
{
	OwnerID = playerID;
	_myPos = InitPos;
	OwnerTankIndex = TankIndex;
}

void AirDrop_Bomb::ProcessMove(float deltaTime)
{
	_myPos.Y -= speed * deltaTime;
}
