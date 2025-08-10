#pragma once
#include "Weapon.h"

class AirDrop_Bomb : public Weapon
{
public:
	AirDrop_Bomb();
	~AirDrop_Bomb();

public:

	virtual void Initialize()					override;
	virtual int  Update(float deltaTime)		override;
	virtual void Late_Update()					override;
	virtual void Release()						override;

public:


	bool isHit() { return _isDead; };
	void SetDead() { _isDead = true; };
	uint8 GetOwnerID() { return OwnerID; };
	uint8		GetOwnerTankIndex() { return OwnerTankIndex; };
	void SetInitData(uint8 OwnerID, uint8 TankIndex, Vec3 InitPos);

	void ProcessMove(float deltaTime);

private:
	USE_LOCK;
	float speed = 50.f;
};

