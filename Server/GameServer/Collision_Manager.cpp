#include "pch.h"
#include "Define.h"
#include "Collision_Manager.h"
#include "ObjectManager.h"
#include "Terrain_Manager.h"



bool CollisionManager::CheckCollision_OBB2D_Circle(const Vec2& circleCenter, float radius, const Vec2& boxCenter, const Vec2 axes[2], const Vec2& halfSize)
{
	Vec2 dir = circleCenter - boxCenter;

	float localX = dir.Dot(axes[0]);
	float localY = dir.Dot(axes[1]);

	float clampedX = std::clamp(localX, -halfSize.X, halfSize.X);
	float clampedY = std::clamp(localY, -halfSize.Y, halfSize.Y);

	Vec2 closestPoint = boxCenter + axes[0] * clampedX + axes[1] * clampedY;
	Vec2 diff = closestPoint - circleCenter;

	return diff.LengthSq() <= radius * radius;
}

bool CollisionManager::CheckCollision_Point_OBB3D(const Vec3& point, const OBB& obb)
{
	Vec3 dir = point - obb.center;

	for (int i = 0; i < 3; ++i)
	{
		float projection = dir.Dot(obb.axis[i]);
		float extent = (i == 0) ? obb.halfSize.X : (i == 1) ? obb.halfSize.Y : obb.halfSize.Z;
		if (fabs(projection) > extent)
			return false;

	}

	return true;
}

bool CollisionManager::Check_Terrain_Collision(GameObject* object)
{
	const Vec3& pos = object->GetPos();
	float terrainHeight = Terrain_Manager::GetInstance().Get_Height(pos.X, pos.Z);
	return pos.Y <= terrainHeight;

}

bool CollisionManager::CheckCollision_Point_Sphere(const Vec3& point, const Vec3& center, float radius)
{
	Vec3 diff = point - center;
	return diff.LengthSq() <= radius * radius;
}

bool CollisionManager::CheckCollision_Point_Sphere2D(const Vec3& point, const Vec3& center, float radius)
{
	Vec2 point2D = { point.X, point.Z };
	Vec2 center2D = { center.X, center.Z };

	Vec2 diff = point2D - center2D;
	return diff.LengthSq() <= radius * radius;
}