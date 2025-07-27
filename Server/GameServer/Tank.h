#pragma once
#include "TransformObject.h"


/*-------------------

	Tank_Info


	//∆¯		380
	//±Ê¿Ã	960
	//≥Ù¿Ã	370


-------------------*/


struct TankSize {

	float Width;
	float Hight;
	float Length;

};


class Tank : public TransformObject
{

public:
	Tank();
	~Tank();

public:
		virtual void Initialize()					override;
		virtual int  Update(float deltaTime)		override;
		virtual void Late_Update()					override;
		virtual void Release()						override;
public:					

	void SetTankState(const Matrix4x4& mat,float PosinAngle, float PotapAngle);
	void SetTankOnlyPos(const Matrix4x4& mat);
	void SetTankOnlyPosin(float PosinAngle, float PotapAngle);

	Tank_INFO GetTankState();
	void UpdateOBBFromTransform();
	OBB2D GetOBB2D() const;
	void Damage(int dmg);
	bool IsDead();
	bool HasPassenger(uint8 playerID) const;
	const OBB& Get_OBB() const { return _obbBox; }



	void AddPassenger(const Room_Ready_Data& data)
	{
		passengers.push_back(data);
	}

	const std::vector<Room_Ready_Data>& GetPassengers() const
	{
		return passengers;
	}

	void SetSpawn(const Matrix4x4& mat, float PosinAngle, float PotapAngle);
	bool isSpawned() { return Spawn; };
	void SetUnSpawn() { Spawn = false; };

	uint64				playerId = 0;
private:
	float				_posinAngle = 0.f;
	float				_potapAngle = 0.f;
	
	
	bool Spawn;

	OBB					_obbBox;
	std::vector<Room_Ready_Data> passengers;
	TankSize MySize = { 3.8f, 3.7f, 9.6f };

};

