#pragma once
#include "Utilities/GameObj.h"
#include "Chr/CPlayer.h"
#include "Chr/Enemy.h"
#include "DebugMgr.h"

#define HITCHECK_PRIORITY 70000
class CHitCheck : public CGameObj
{
private:
	CPlayer* player;
	CEnemy* enemy;
	CDebugMgr* debug;
	TCHAR font[50];
	RECT rc;
	int id;
	float damege;
	bool flag;
	bool hitflag;
public:
	CHitCheck();
	~CHitCheck();
	bool Hit(float center_x, float center_y, float scale_x, float scale_y, float offset_x, float offset_y,
		float center_x1, float center_y1, float scale_x1, float scale_y1, float offset_x1, float offset_y1);
	bool HitTest(float in_x, float in_y, float in_z, float in_radius, float in_x1, float in_y1, float in_z1, float in_radius1);
	void Set(int in_id, float in_damege, bool in_flag);
protected:
	virtual void Init();
	virtual void Update();
	virtual void Render();
};