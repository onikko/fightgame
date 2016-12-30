#pragma once
#include "Utilities/GameObj.h"
#include "Chr/CPlayer.h"
#include "Chr/Enemy.h"

#define ADJUST_PRIORITY 75000

class CAdjust: public CGameObj
{
private:
	D3DXVECTOR3 center;
	D3DXVECTOR3 unit;
	float dist;
public:
	CPlayer* player;
	CEnemy* enemy;
	CAdjust();
	~CAdjust();
protected:
	virtual void Init();
	virtual void Update();
	virtual void Render();
};

