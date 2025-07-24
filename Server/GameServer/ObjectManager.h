#pragma once
#include "GameObject.h"
class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

	void Add_Object(ObjectID eID, GameObject* pGameObject) {
		if (pGameObject)
			m_ObjectList[eID].push_back(pGameObject);
	}

	vector<GameObject*>* Get_List(ObjectID eID) {
		if (!List_Empty(eID))
			return &m_ObjectList[eID];
		return nullptr;
	}

	bool List_Empty(ObjectID eID) {
		return m_ObjectList[eID].empty();
	}

	int Update(float deltaTime);
	void Late_Update();
	void Release();

private:
	std::vector<GameObject*> m_ObjectList[OBJ_END];
};

