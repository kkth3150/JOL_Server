#pragma once

struct Vec2;
struct Vec3;
struct OBB;

class GameObject

class CollisionManager
{
private:
	CollisionManager() = default;
	~CollisionManager() = default;

public:
	static CollisionManager* GetInstance()
	{
		static CollisionManager instance;
		return &instance;
	}

	// 2D OBB vs Circle
	bool CheckCollision_OBB2D_Circle(const Vec2& circleCenter, float radius, const Vec2& boxCenter, const Vec2 axes[2], const Vec2& halfSize);

	// 3D Point vs OBB
	bool CheckCollision_Point_OBB3D(const Vec3& point, const OBB& obb);

	bool Check_Terrain_Collision(GameObject* GameObejct);


};
