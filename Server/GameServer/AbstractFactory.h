#pragma once
#include "GameObject.h"

template<typename T>
class CAbstractFactory
{
public:
	CAbstractFactory() {}
	~CAbstractFactory() {}

public:
	static GameObject* Create()
	{
		GameObject* pGameObject = new T;
		pGameObject->Initialize();

		return pGameObject;
	}

	static GameObject* Create(float fX, float fY, float fZ)
	{
		GameObject* pGameObject = new T;
		pGameObject->SetPos(fX, fY, fZ);
		pGameObject->Initialize();

		return pGameObject;
	}


};