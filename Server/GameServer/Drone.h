#pragma once
#include "TransformObject.h"

class Drone : public TransformObject
{

public:
	Drone();
	~Drone();

public:
	virtual void Initialize()					override;
	virtual int  Update(float deltaTime)		override;
	virtual void Late_Update()					override;
	virtual void Release()						override;

public:

	void SetDroneState(const Matrix4x4& mat);
	Drone_INFO GetDroneState();
	void Damage(int dmg);

	bool IsDead();

	void SetSpawn(const Matrix4x4& mat);

	void AddPassenger(const Room_Ready_Data& data)
	{
		passengers.push_back(data);
	}

	const std::vector<Room_Ready_Data>& GetPassengers() const
	{
		return passengers;
	}


	std::vector<Room_Ready_Data> passengers;
	bool Spawn;

};

