#include "pch.h"
#include "ObjectManager.h"


ObjectManager::ObjectManager()
{
}

ObjectManager::~ObjectManager()
{
	Release();
}


int ObjectManager::Update(float deltaTime)
{
	for (size_t i = 0; i < OBJ_END; ++i)
	{
		auto& vec = m_ObjectList[i];
		for (size_t j = 0; j < vec.size(); )
		{
			int iResult = vec[j]->Update(deltaTime);

			if (1 == iResult)
			{
				Safe_Delete(vec[j]);
				vec.erase(vec.begin() + j);
			}
			else
			{
				++j;
			}
		}
	}

	return 0;
}

void ObjectManager::Late_Update(void)
{
	for (size_t i = 0; i < OBJ_END; ++i)
	{
		auto& vec = m_ObjectList[i];
		for (auto& obj : vec)
		{
			if (obj)
				obj->Late_Update();
		}
	}
}

void ObjectManager::Release(void)
{
	for (size_t i = 0; i < OBJ_END; ++i)
	{
		auto& vec = m_ObjectList[i];
		for (auto& obj : vec)
			Safe_Delete(obj);
		vec.clear();
	}
}
