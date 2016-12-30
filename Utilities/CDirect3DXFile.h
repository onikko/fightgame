//*****************************************************************************
//!	@file	CDirect3DXFile.h
//!	@brief	
//!	@note	Xファイルクラス
//!	@author
//*****************************************************************************
#pragma once

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include "GameObj.h"

//=============================================================================
//!	@class	CDirect3DXFile
//!	@brief	Xファイルクラス
//=============================================================================
class CDirect3DXFile : public CGameObj
{
private:
	LPD3DXMESH					m_lpmesh;				// メッシュインターフェイス
	D3DMATERIAL9*				m_lpmeshmaterials;		// モデルデータのマテリアル
	LPDIRECT3DTEXTURE9*			m_lpmeshtextures;		// モデルデータのテクスチャ
	DWORD						m_nummaterial;			// マテリアル数
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