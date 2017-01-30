#pragma once
#include "../Utilities/GameObj.h"

#define GAUGE_TIME 30

class CGauge : public CGameObj{
public:
	float Value;
	float PrevValue;
	float Value1;
	int Index;
	int Wait;
	int WinCount;
	CGauge(int index) : Index(index)
	{
		WinCount = 0;
		Reset();
	}
	void Reset();
	void Damage(float v);
	void Move();
	void Draw();
};

