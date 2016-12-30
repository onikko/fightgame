#pragma once

#include "../Utilities/GameObj.h"
#include "../Camera/CamaraTPS.h"
#include "../CBillBoard.h"
#include "../Utilities/CDebugFont.h"
#include <time.h>

#define GAUGE_PRIORITY 110000
#define TIME_PRIORITY 105000
class CUserInterface : public CGameObj
{
private:
	CDebugFont* m_font;
	bool flag;
public:
	CUserInterface();
	~CUserInterface();
};

class CTime : public CUserInterface{
private:
	CCamaraTPS* camera;
	CBillBoard BillBoard;
	D3DXVECTOR3 vec3Trans;
	LPDIRECT3DTEXTURE9 m_pd3dTex;
	DWORD old_time = 0;
	int m_time;
	int m_cnt;
	int one;
	int ten;
public:
	CTime();
	~CTime();
	DWORD GetMMTime(void);
	void BatchProcessing(D3DXVECTOR3 in, int num, float x, float y, const D3DXMATRIX& matView, LPDIRECT3DTEXTURE9 tex);
protected:
	virtual void Init();
	virtual void Update();
	virtual void Render();
};