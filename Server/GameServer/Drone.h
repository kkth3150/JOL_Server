#pragma once
#include "TransformObject.h"

struct DroneRot {
	float Yaw;
	float Roll;
	float Pitch;
};

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

	void SetDroneState(Vec3 Pos, float Yaw, float Roll, float Pitch);
	Vec3 GetDronePos();
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

	DroneRot My_DroneRot;
	Vec3	My_DronePos;
};

