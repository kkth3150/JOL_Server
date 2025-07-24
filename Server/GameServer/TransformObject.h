#pragma once
#include "GameObject.h"
class TransformObject : public GameObject
{
public:
	TransformObject();
	virtual ~TransformObject();

protected:
	Matrix4x4 TransformMatrix;

public:
	void SetTransform(const Matrix4x4& mat) { TransformMatrix = mat; }
	const Matrix4x4& GetTransform() const { return TransformMatrix; }

	// 필요하다면 Pos도 동기화할 수 있도록
	void SyncPosFromMatrix() {
		_myPos.X = TransformMatrix.m[3][0];
		_myPos.Y = TransformMatrix.m[3][1];
		_myPos.Z = TransformMatrix.m[3][2];
	}

};

