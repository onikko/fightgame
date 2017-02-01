#pragma once
#include "Utilities/GameObj.h"
#include "Utilities/CDebugFont.h"
#include "CBillBoard.h"

#include "Camera/CamaraTPS.h"

#define DEBUG_PRIORITY 115000
#define FPS_COUNT_X 5
#define FPS_COUNT_Y 5
class CDebugMgr : public CGameObj
{
private:
	CCamaraTPS* camera;
	CDebugFont* m_font;
	TCHAR time_font[50];
	TCHAR str1[256];
	TCHAR work_font[50];
	RECT rc;
	CBillBoard BillBoard;
	D3DXVECTOR3 vec3Trans;
	LPDIRECT3DTEXTURE9 m_pd3dTex;
	CTexture* t;
	bool flag;
	int input;
	int work_id;
	//FPSŒvŽZ•\Ž¦
	DWORD time;
	int frame;
	int av_frame;
	int fps[3600];
	int avg;
	int first2;
	int old_input;
public:
	CDebugMgr();
	~CDebugMgr();
	void Draw(char* font, int x, int y);
	void SetDeta(char*, bool);
	void SetInput(int);
	void SetId(int);
protected:
	virtual void Init();
	virtual void Update();
	virtual void Render();
};

