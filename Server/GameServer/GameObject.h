#pragma once
#include "Define.h"

class GameObject
{

public:

	GameObject();
	~GameObject();

public:
	virtual void Initialize()					PURE;
	virtual int	 Update(float deltaTime)		PURE;
	virtual void Late_Update()					PURE;
	virtual void Release()						PURE;

protected:

	Vec3	_myPos;
	Size	_mySize;
	bool	_isDead;
	uint8	_hp;

public:

	void SetPos(float a, float b, float c) { _myPos.X = a, _myPos.Y = b, _myPos.Z = c; };
	const Vec3 GetPos() { return _myPos; };
};

