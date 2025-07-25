#pragma once
#include "GameObject.h"

class Tree : public GameObject
{
public:

	Tree();
	~Tree();

public:
	virtual void Initialize()					override;
	virtual int  Update(float deltaTime)		override;
	virtual void Late_Update()					override;
	virtual void Release()						override;

private:


};

