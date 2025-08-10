#include "pch.h"
#include "Tank.h"

Tank::Tank()
{
	Spawn = true;
	_hp = 100;
}

Tank::~Tank()
{
}

void Tank::Initialize()
{

}

int Tank::Update(float deltaTime)
{
	SyncPosFromMatrix();
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

void Tank::SetTankOnlyPos(const Matrix4x4& mat)
{
	__super::SetTransform(mat);
}

void Tank::SetTankOnlyPosin(float PosinAngle, float PotapAngle)
{
	_potapAngle = PotapAngle;
	_posinAngle = PosinAngle;
}

Tank_INFO Tank::GetTankState()
{

	Tank_INFO myInfo = {

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

OBB2D Tank::GetOBB2D() const
{
	return {
		Vec2(_obbBox.center.X, _obbBox.center.Z),
		{ Vec2(_obbBox.axis[0].X, _obbBox.axis[0].Z).GetNormalized(),
		  Vec2(_obbBox.axis[2].X, _obbBox.axis[2].Z).GetNormalized() },
		Vec2(_obbBox.halfSize.X, _obbBox.halfSize.Z)
	};
}

void Tank::Damage(int dmg)
{
	_hp -= dmg;
	if (_hp < 0)
		_hp = 0;
}

bool Tank::IsDead() 
{
	
	return _hp <= 0;

}

void Tank::SetSpawn(const Matrix4x4& mat, float PosinAngle, float PotapAngle)
{

	__super::SetTransform(mat);
	_potapAngle = PotapAngle;
	_posinAngle = PosinAngle;
	Spawn = true;
	_hp = 100;
}
