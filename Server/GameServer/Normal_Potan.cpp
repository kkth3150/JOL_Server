#include "pch.h"
#include "Normal_Potan.h"
#include "Terrain_Manager.h"


void Normal_Potan::Initialize()
{
    myWeaponID = WEAPON_NPOTAN;
    _isDead = false;
 
}

int Normal_Potan::Update(float deltaTime)
{
    if (_isDead) {

        return 1;
    }

    Process_Move(deltaTime);

    return 0;

}

void Normal_Potan::Late_Update()
{

}

void Normal_Potan::Release()
{
}

void Normal_Potan::SetInitData(Vec3& normalizedDirection, Vec3& startPos,uint8 TankIndex ,uint8 ID,bool blue)
{

    OwnerID = ID;
    _myPos = startPos;
    direction = normalizedDirection;   // 정규화된 방향벡터 멤버 변수로 저장                    // 초기 속도 멤버 변수로 고정
    velocity = {
        direction.X * speed,
        direction.Y * speed,
        direction.Z * speed
    };
    
    OwnerTankIndex = TankIndex;
    _isBlue = blue;
}

void Normal_Potan::Process_Move(float deltaTime)
{
    Vec3 gravity = { 0.f, -6.8f, 0.f };

    // 중력 가속도를 velocity에 더함 (v = v + a * dt)
    velocity.X += gravity.X * deltaTime;
    velocity.Y += gravity.Y * deltaTime;
    velocity.Z += gravity.Z * deltaTime;

    // 위치를 velocity 방향으로 deltaTime 만큼 이동 (p = p + v * dt)
    _myPos.X += velocity.X * deltaTime;
    _myPos.Y += velocity.Y * deltaTime;
    _myPos.Z += velocity.Z * deltaTime;

    const float BOUND_LIMIT = 1900.f; // 임계값
    if (_myPos.X < -BOUND_LIMIT || _myPos.X > BOUND_LIMIT ||
        _myPos.Z < -BOUND_LIMIT || _myPos.Z > BOUND_LIMIT)
    {
        _isDead = true;
    }
}

bool Normal_Potan::Check_Collision()
{

    if (!_isDead) {
        if (Collision_Terrain()) {
            return 1;
        }
    }

    return 0;
}

bool Normal_Potan::Collision_Terrain()
{
    float terrainHeight = Terrain_Manager::GetInstance().Get_Height(_myPos.X, _myPos.Z);

    return (_myPos.Y <= terrainHeight);
}


