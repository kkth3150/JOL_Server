#pragma once
#include "TransformObject.h"


/*-------------------

	Tank_Info


	//∆¯		380
	//±Ê¿Ã	960
	//≥Ù¿Ã	370


-------------------*/

class Tank : public TransformObject
{

public:
	Tank();
	~Tank();

public:
		virtual void Initialize()					override;
		virtual int  Update(float deltaTime)		override;
		virtual void Late_Update()					override;
		virtual void Release()						override;
public:					

	void SetTankState(const Matrix4x4& mat,float PosinAngle, float PotapAngle);
	Tank_INFO GetTankState();
	const OBB& Get_OBB() const { return _obbBox; }

	uint64				playerId = 0;
private:
	float				_posinAngle = 0.f;
	float				_potapAngle = 0.f;
	
	OBB					_obbBox;
	   

};

