//*****************************************************************************
//!	@file	CDirect3DXFile.h
//!	@brief	
//!	@note	X�t�@�C���N���X
//!	@author
//*****************************************************************************
#pragma once

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include "GameObj.h"

//=============================================================================
//!	@class	CDirect3DXFile
//!	@brief	X�t�@�C���N���X
//=============================================================================
class CDirect3DXFile : public CGameObj
{
private:
	LPD3DXMESH					m_lpmesh;				// ���b�V���C���^�[�t�F�C�X
	D3DMATERIAL9*				m_lpmeshmaterials;		// ���f���f�[�^�̃}�e���A��
	LPDIRECT3DTEXTURE9*			m_lpmeshtextures;		// ���f���f�[�^�̃e�N�X�`��
	DWORD						m_nummaterial;			// �}�e���A����
public:
	CDirect3DXFile(){
		m_lpmeshmaterials=NULL;
		m_lpmeshtextures=NULL;
		m_lpmesh=NULL;
	}

	CDirect3DXFile(LPCTSTR name){
		m_lpmeshmaterials = NULL;
		m_lpmeshtextures = NULL;
		m_lpmesh = NULL;
		LoadXFile(name);
	}

	~CDirect3DXFile()
	{
		UnLoadXFile();
	}

	bool LoadXFile(LPCTSTR xfilename);
	LPD3DXMESH GetMesh() const;
	void UnLoadXFile();
	void Draw();
	void DrawWithAxis();
	void DrawAxis();
};

//******************************************************************************
//	End of file.
//******************************************************************************