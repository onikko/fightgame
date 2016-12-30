//============================================================
//!	@file		CSkinMeshEffect.cpp
//!	@brief		スキンメッシュエフェクト管理クラス
//!	@note		LastEdited 2015/12/01
//!	@author		ＯＨ教務部ゲーム学科有志
//============================================================

#include "CSkinMeshEffect.h"

bool CSkinMeshEffect::LoadEffect( LPCSTR effectpath )
{
	CEffectFactory::Instance().CreateEffect(
		eSKIN_MESH,
		0,
		CGameObj::Instance().GetDXDevice(),
		effectpath,
		D3DXSHADER_DEBUG,
		m_effect );

	CGeneralFactory<D3DXHANDLE>::Instance().Create( eSKIN_MESH, eHANDLE_TECH, m_hTech );
	*m_hTech = m_effect->GetTechniqueByName("BlendTech");
		
	CGeneralFactory<D3DXHANDLE>::Instance().Create( eSKIN_MESH, eHANDLE_WORLD, m_hWorld );
	*m_hWorld = m_effect->GetParameterByName(0, "world");
		
	CGeneralFactory<D3DXHANDLE>::Instance().Create( eSKIN_MESH, eHANDLE_VIEW, m_hView);
	*m_hView = m_effect->GetParameterByName(0, "view");
		
	CGeneralFactory<D3DXHANDLE>::Instance().Create( eSKIN_MESH, eHANDLE_PROJ, m_hProj );
	*m_hProj = m_effect->GetParameterByName(0, "proj");
		
	CGeneralFactory<D3DXHANDLE>::Instance().Create( eSKIN_MESH, eHANDLE_AMBIENT, m_hAmbient );
	*m_hAmbient = m_effect->GetParameterByName(0, "ambient");
		
	CGeneralFactory<D3DXHANDLE>::Instance().Create( eSKIN_MESH, eHANDLE_DIFFUSE, m_hDiffuse );
	*m_hDiffuse = m_effect->GetParameterByName(0, "diffuse");
		
	CGeneralFactory<D3DXHANDLE>::Instance().Create( eSKIN_MESH, eHANDLE_LIGHTDIR, m_hLightDir );
	*m_hLightDir = m_effect->GetParameterByName(0, "lightDir");
		
	CGeneralFactory<D3DXHANDLE>::Instance().Create( eSKIN_MESH, eHANDLE_TEXTURE, m_hTexture );
	*m_hTexture = m_effect->GetParameterByName(0, "tex");

	CGeneralFactory<D3DXHANDLE>::Instance().Create( eSKIN_MESH, eHANDLE_SPECULAR, m_hSpecular );
	*m_hSpecular = m_effect->GetParameterByName(0, "specular");

	CGeneralFactory<D3DXHANDLE>::Instance().Create( eSKIN_MESH, eHANDLE_POWER, m_hPower );
	*m_hPower = m_effect->GetParameterByName(0, "power");

	CGeneralFactory<D3DXHANDLE>::Instance().Create( eSKIN_MESH, eHANDLE_CAMERAPOS, m_hCamerapos );
	*m_hCamerapos = m_effect->GetParameterByName(0, "camerapos");
	
	CGeneralFactory<D3DXHANDLE>::Instance().Create( eSKIN_MESH, eHANDLE_BLOOMPOWER, m_hBloomPower );
	*m_hBloomPower = m_effect->GetParameterByName(0, "lightbloompower");
	if (!*m_hTech || !*m_hWorld || !*m_hView ||
		!*m_hProj || !*m_hAmbient || !*m_hDiffuse ||
		!*m_hLightDir || !*m_hTexture)
		return false;

	return true;
}