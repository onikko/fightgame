#pragma once
#include "LoadData.h"
#include "../SimpleCamera.h"
#include <mutex>

class CSceneMgr
{
public:
	CSceneMgr();
	~CSceneMgr();
};

// タイトルモード
class CTitle : public CGameObj
{
private:
	CDebugFont* m_font;
	CDebugFont* font;
	CBillBoard billboard;
	CSimpleCamera* camera;
	D3DXMATRIX a;
	LPDIRECT3DTEXTURE9 m_pd3dTex;
	struct MyVertex{
		float		x, y, z;
		D3DCOLOR	color;
		float		tu, tv;
	};
	MyVertex		m_Vertex[4];			// ビルボードの頂点座標
protected:
	virtual void Init();
	virtual void Input();
	virtual void Render();
public:
	CTitle();
	~CTitle();
	void title();
	void BatchProcessing(D3DXVECTOR3 in, float x, float y, const D3DXMATRIX& matView, LPDIRECT3DTEXTURE9 tex);
};


// ゲームクリア・ゲームオーバーモード
#define CONTINUE_PRIORITY 100000

class CConfig : public CGameObj
{
public:
	CConfig(bool gameclear);
private:
	bool clear;
protected:
	virtual void Init();
	virtual void Input();
	virtual void Render();
};

class CLoadStage : public CLoadData
{
protected:
	virtual void Init();
	static void LoadThread(void *data);
	static void LoadThreadPlayer1(void *data);
	static void LoadThreadPlayer2(void *data);
};

class CLoadTitle : public CLoadData
{
protected:
	virtual void Init();
	static void LoadThread(void *data);
	static void LoadThreadPlayer1(void *data);
	static void LoadThreadPlayer2(void *data);
};