#pragma once
#include "GameObject.h"
class Weapon : public GameObject
{

public:
	Weapon();
	~Weapon();

public:
	virtual void Initialize()					PURE;
	virtual int  Update(float deltaTime)		PURE;
	virtual void Late_Update()					PURE;
	virtual void Release()						PURE;

protected:
	


	float Move_Speed;
	float InitPos;


	WEAPON_ID	myWeaponID;
	uint8		OwnerID;
	uint8		OwnerTankIndex;
	Vec3		hitPos;


};

