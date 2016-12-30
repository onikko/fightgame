//============================================================
//!	@file		CGraphicsMeshBone.h
//!	@brief		グラフィックスメッシュクラス
//!	@note		LastEdited 2015/1/31
//!	@author		ＯＨ教務部ゲーム学科有志
//============================================================

// インクルードガード
#ifndef _INC_CGRAPHICSMESHBONE_H
#define _INC_CGRAPHICSMESHBONE_H

#include <vector>
#include "../Factory.h"
#include "../CEffect.h"
#include "../SkinMesh/CSkinMeshEffect.h"
#include "CGraphicsOrderBase.h"

//============================================================
//!	@class	CGraphicsMeshBone
//!	@brief	グラフィックスメッシュボーンクラス
//============================================================
class CGraphicsMeshBone : public CGraphicsOrderBase
{
private:
	LPD3DXMESH						m_mesh;
	LPDIRECT3DTEXTURE9				m_texture;
	D3DMATERIAL9					m_material;
	std::vector<D3DXMATRIX>			m_worldboneMatrix;
	sp< CSkinMeshEffect >			m_effect;
	UINT							m_id;
	D3DXMATRIX						m_view;
	D3DXMATRIX						m_proj;
	D3DXMATRIX						m_camera;
public:
	bool SetMesh(
		LPD3DXMESH* mesh,
		D3DMATERIAL9* material,
		LPDIRECT3DTEXTURE9 texture,
		std::vector<D3DXMATRIX> boneMatrix,
		sp< CEffect > effect,
		D3DXMATRIX view,
		D3DXMATRIX proj,
		D3DXMATRIX m_camera,
		UINT id );
	void Draw( LPDIRECT3DDEVICE9 _device );
};

#endif

//EOF