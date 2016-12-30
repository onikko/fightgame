//============================================================
//!	@file		CGraphicsMeshBone.h
//!	@brief		グラフィックスメッシュクラス
//!	@note		LastEdited 2015/1/31
//!	@author		ＯＨ教務部ゲーム学科有志
//============================================================

#include "CGraphicsMeshBone.h"

bool CGraphicsMeshBone::SetMesh(
	LPD3DXMESH* mesh,
	D3DMATERIAL9* material,
	LPDIRECT3DTEXTURE9 texture,
	std::vector<D3DXMATRIX> boneMatrix,
	sp< CEffect > effect,
	D3DXMATRIX view,
	D3DXMATRIX proj,
	D3DXMATRIX camera,
	UINT id )
{
	//--プログラムの流れ--
	m_mesh = *mesh;
	m_material = *material;
	if( m_material.Ambient.a < 1.0f || m_material.Diffuse.a<1.0f ){
		m_priority = eGRAPHICS_WITH_ALPHA;
	}else{
		m_priority = eGRAPHICS_NON_ALPHA;
	}
	
	if(!(m_effect.DownCast( effect )))
		return false;
	m_view = view;
	m_proj = proj;
	m_texture = texture;
	m_worldboneMatrix = boneMatrix;
	m_camera = camera;
	m_id = id;
	return true;
}

void CGraphicsMeshBone::Draw( LPDIRECT3DDEVICE9 _device )
{
	vector<D3DXMATRIX> worldmtx;
	D3DXVECTOR3 light_dir(0.0f, 1.0f, 1.0f );

	m_effect->GetEffect()->SetTechnique(m_effect->GetTechHandle());
	m_effect->GetEffect()->SetMatrix(m_effect->GetViewHandle(), &m_view);
	m_effect->GetEffect()->SetMatrix(m_effect->GetProjHandle(), &m_proj);
	m_effect->GetEffect()->SetFloatArray(m_effect->GetLightDirHandle(), (const float*)&light_dir, 3);
//	m_effect->GetEffect()->SetFloatArray(m_effect->GetCameraPosHandle(), (const float*)m_camera->GetFrom(), 3 ); // 20151205
	m_effect->GetEffect()->SetFloat(m_effect->GetBloomPowerHandle(), 20.0f );
	m_effect->GetEffect()->Begin( 0, 0 );
	m_effect->GetEffect()->SetMatrixArray(m_effect->GetWorldHandle(), &(m_worldboneMatrix[0]), m_worldboneMatrix.size());
	m_effect->GetEffect()->SetFloatArray(m_effect->GetAmbientHandle(), (const float*)&m_material.Ambient, 4);
	m_effect->GetEffect()->SetFloatArray(m_effect->GetDiffuseHandle(), (const float*)&m_material.Diffuse, 4);
	//m_effect->GetEffect()->SetFloatArray(m_effect->GetSpecularHandle(), (const float*)&m_material.Specular, 4 );
	m_effect->GetEffect()->SetFloatArray(m_effect->GetSpecularHandle(), (const float*)&D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f ), 4 );
	m_effect->GetEffect()->SetFloat(m_effect->GetPowerHandle(), 20 );
	m_effect->GetEffect()->SetTexture(m_effect->GetTextureHandle(), m_texture);
	if (m_texture!= NULL ) m_effect->GetEffect()->BeginPass(0);
	else m_effect->GetEffect()->BeginPass(1);
	m_mesh->DrawSubset(m_id);
	m_effect->GetEffect()->EndPass();
	m_effect->GetEffect()->End();
}