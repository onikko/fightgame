#pragma once

#include "Utilities/GameObj.h"
#include "Chr/CPlayer.h"
#include "Utilities/SafeDelete.h"

class CHitBox : public CGameObj
{
private:
	LPD3DXMESH					m_lpmesh;				// メッシュインターフェイス
	D3DMATERIAL9*				m_lpmeshmaterials;		// モデルデータのマテリアル
	LPDIRECT3DTEXTURE9*			m_lpmeshtextures;		// モデルデータのテクスチャ
	DWORD						m_nummaterial;			// マテリアル数
	D3DXCOLOR ColorMultiplier, ColorAddition;			// 色
public:
	CHitBox(){
		m_lpmeshmaterials = NULL;
		m_lpmeshtextures = NULL;
		m_lpmesh = NULL;
	}

	CHitBox(LPCTSTR name){
		m_lpmeshmaterials = NULL;
		m_lpmeshtextures = NULL;
		m_lpmesh = NULL;
		LoadXFile(name);
	}

	~CHitBox()
	{
		UnLoadXFile();
	}

	void SetColor(const D3DXCOLOR& multiplier, const D3DXCOLOR& addition) {
		ColorMultiplier = multiplier;
		ColorAddition = addition;
	}

	bool LoadXFile(LPCTSTR xfilename);
	LPD3DXMESH GetMesh() const;
	void UnLoadXFile();
	void Update(D3DXMATRIX&, D3DXMATRIX, D3DXVECTOR3, float);
	void Draw();
	void Draw(const D3DXMATRIX& mat);
};

class sample : public CGameObj
{
private:
	// カスタム頂点
	struct Vertex {
		D3DXVECTOR3 pos;
		D3DCOLOR diffuse;
		float u, v;
	};
	// ビルボード頂点
	Vertex billboard[4];
	IDirect3DVertexBuffer9 *buffer = 0;
	D3DXMATRIX worldMat;
	D3DXVECTOR3 m_wpos;
	D3DXMATRIX	maty;
	CPlayer* player;
public:
	sample();
	~sample();
	void SetColor();
	bool LoadXFile();
	LPD3DXMESH GetMesh() const;
	void UnLoadXFile();
	void Update();
	void Draw();
};