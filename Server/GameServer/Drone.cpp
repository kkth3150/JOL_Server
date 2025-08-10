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

void Drone::SetDroneState(const Matrix4x4& mat)
{
	__super::SetTransform(mat);

}

Drone_INFO Drone::GetDroneState()
{
	Drone_INFO myInfo = {

		TransformMatrix,
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