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

    // 중심 좌표 (탱크 중간 높이까지 올림)
	Matrix4x4& mat = TransformMatrix;

	// 중심 좌표: 탱크 위치 + 높이의 절반
	_obbBox.center = Vec3(
		mat.m[3][0],             // X (world pos X)
		mat.m[3][1] + 185.f,     // Y (중심은 높이 370의 절반만큼 위)
		mat.m[3][2]              // Z (world pos Z)
	);

	// 회전 축 벡터 추출
	_obbBox.axis[0] = Vec3(mat.m[0][0], mat.m[0][1], mat.m[0][2]); // Right (X축)
	_obbBox.axis[1] = Vec3(mat.m[1][0], mat.m[1][1], mat.m[1][2]); // Up (Y축)
	_obbBox.axis[2] = Vec3(mat.m[2][0], mat.m[2][1], mat.m[2][2]); // Forward (Z축)

	// 반사이즈 설정 (탱크 크기: 폭 380, 높이 370, 길이 960)
	_obbBox.halfSize = Vec3(190.f, 185.f, 480.f); // 반씩

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

		TransformMatrix,
		_posinAngle,
		_potapAngle,
		_hp

	};

	return myInfo;
}
