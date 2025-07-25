#include "pch.h"
#include "Tank.h"

Tank::Tank()
{
}

Tank::~Tank()
{
}

void Tank::Initialize()
{

}

int Tank::Update(float deltaTime)
{

	UpdateOBBFromTransform();
	return 0;
}

void Tank::Late_Update()
{
}

void Tank::Release()
{

}

void Tank::SetTankState(const Matrix4x4& mat, float PotapAngle, float PosinAngle)
{
	__super::SetTransform(mat);
	_potapAngle = PotapAngle;
	_posinAngle = PosinAngle;

}

Tank_INFO Tank::GetTankState()
{

	Tank_INFO myInfo = {

		playerId,
		TransformMatrix,
		_posinAngle,
		_potapAngle,
		_hp

	};

	return myInfo;
}

void Tank::UpdateOBBFromTransform()
{
	Matrix4x4& mat = TransformMatrix;

	_obbBox.center = Vec3(
		mat.m[3][0],
		mat.m[3][1] + MySize.Hight * 0.5f,
		mat.m[3][2]
	);

	_obbBox.axis[0] = Vec3(mat.m[0][0], mat.m[0][1], mat.m[0][2]);
	_obbBox.axis[1] = Vec3(mat.m[1][0], mat.m[1][1], mat.m[1][2]);
	_obbBox.axis[2] = Vec3(mat.m[2][0], mat.m[2][1], mat.m[2][2]);

	for (int i = 0; i < 3; ++i)
		_obbBox.axis[i].Normalize();

	_obbBox.halfSize = Vec3(
		MySize.Width * 0.5f,
		MySize.Hight * 0.5f,
		MySize.Length * 0.5f
	);
}