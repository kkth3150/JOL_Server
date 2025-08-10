#pragma once
#include "Weapon.h"

class Normal_Potan : public Weapon
{
public:
	virtual void Initialize()					override;
	virtual int  Update(float deltaTime)		override;
	virtual void Late_Update()					override;
	virtual void Release()						override;

public:

	void SetInitData(Vec3& normalizedDirection, Vec3& startPos,uint8 TankIndex ,uint8 OwnerID,bool isBlueTeam);
	uint8 GetOwnerID() { return OwnerID; };
	void Process_Move(float deltaTime);

	bool Check_Collision();
	bool isHit() {
		return _isDead;
	}

	bool Check_Terrain_Collision(Normal_Potan* potan);

	bool Collision_Terrain();
	void SetDead() { _isDead = true; };

	uint8		GetOwnerTankIndex() { return OwnerTankIndex; };

private:
	USE_LOCK;
	float speed = 150.f;

	bool isActive = false;
	Vec3 direction;
	Vec3 velocity;

	
};

