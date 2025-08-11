#include "pch.h"
#include "Drone.h"

Drone::Drone()
{
	Spawn = true;
	_hp = 100;
}

Drone::~Drone()
{
}

void Drone::Initialize()
{
}

int Drone::Update(float deltaTime)
{
	SyncPosFromMatrix();
	return 0;
}

void Drone::Late_Update()
{
}

void Drone::Release()
{
}

void Drone::SetDroneState(Vec3 Pos, float Yaw, float Roll, float Pitch)
{
	Matrix4x4 mat = __super::GetTransform();
	mat.m[3][0] = Pos.X;
	mat.m[3][1] = Pos.Y;
	mat.m[3][2] = Pos.Z;

	My_DronePos = Pos;

	__super::SetTransform(mat);

	My_DroneRot.Yaw = Yaw;
	My_DroneRot.Roll = Roll;
	My_DroneRot.Pitch = Pitch;

}

Drone_INFO Drone::GetDroneState()
{
	Drone_INFO myInfo = {

		My_DronePos,
		My_DroneRot.Yaw,
		My_DroneRot.Roll,
		My_DroneRot.Pitch,
		_hp

	};

	return myInfo;
}

void Drone::Damage(int dmg)
{

	_hp -= dmg;
	if (_hp < 0)
		_hp = 0;
}


bool Drone::IsDead()
{
	return _hp <= 0;
}


void Drone::SetSpawn(const Matrix4x4& mat)
{
	__super::SetTransform(mat);
	Spawn = true;
	_hp = 100;
}