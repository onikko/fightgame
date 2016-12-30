#pragma once

#include "Utilities/GameObj.h"
#include "Chr/CPlayer.h"
#include "Utilities/SafeDelete.h"

class CHitBox : public CGameObj
{
private:
	LPD3DXMESH					m_lpmesh;				// ���b�V���C���^�[�t�F�C�X
	D3DMATERIAL9*				m_lpmeshmaterials;		// ���f���f�[�^�̃}�e���A��
	LPDIRECT3DTEXTURE9*			m_lpmeshtextures;		// ���f���f�[�^�̃e�N�X�`��
	DWORD						m_nummaterial;			// �}�e���A����
	D3DXCOLOR ColorMultiplier, ColorAddition;			// �F
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
	// �J�X�^�����_
	struct Vertex {
		D3DXVECTOR3 pos;
		D3DCOLOR diffuse;
		float u, v;
	};
	// �r���{�[�h���_
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