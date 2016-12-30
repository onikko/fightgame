//============================================================
//!	@file		CSkinMeshEffect
//!	@brief		スキンメッシュシェーダ読み込みクラス
//!	@note		LastEdited 2015/12/1
//!	@author		ＯＨ教務部ゲーム学科有志
//============================================================

// インクルードガード
#ifndef _INC_CSKINMESHEFFECT_H
#define _INC_CSKINMESHEFFECT_H

#include "../CEffect.h"

//============================================================
//!	@class	CSkinMeshEffect
//!	@brief	スキンメッシュシェーダ読み込みクラス
//============================================================
class CSkinMeshEffect : public CEffect
{
private:
	cp<ID3DXEffect> m_effect;
	sp<D3DXHANDLE> m_hTech;
	sp<D3DXHANDLE> m_hWorld;
	sp<D3DXHANDLE> m_hView;
	sp<D3DXHANDLE> m_hProj;
	sp<D3DXHANDLE> m_hAmbient;
	sp<D3DXHANDLE> m_hDiffuse;
	sp<D3DXHANDLE> m_hLightDir;
	sp<D3DXHANDLE> m_hTexture;
	sp<D3DXHANDLE> m_hSpecular;
	sp<D3DXHANDLE> m_hPower;
	sp<D3DXHANDLE> m_hCamerapos;
	sp<D3DXHANDLE> m_hBloomPower;

public:
	CSkinMeshEffect( void ){};
	bool LoadEffect( LPCSTR effectpath );
	cp<ID3DXEffect> GetEffect( void ){
		return m_effect;
	}
	D3DXHANDLE GetTechHandle( void ){
		return *m_hTech;
	}
	D3DXHANDLE GetWorldHandle( void ){
		return *m_hWorld;
	}
	D3DXHANDLE GetViewHandle( void ){
		return *m_hView;
	}
	D3DXHANDLE GetProjHandle( void ){
		return *m_hProj;
	}
	D3DXHANDLE GetAmbientHandle( void ){
		return *m_hAmbient;
	}
	D3DXHANDLE GetDiffuseHandle( void ){
		return *m_hDiffuse;
	}
	D3DXHANDLE GetLightDirHandle( void ){
		return *m_hLightDir;
	}
	D3DXHANDLE GetTextureHandle( void ){
		return *m_hTexture;
	}
	D3DXHANDLE GetSpecularHandle( void ){
		return *m_hSpecular;
	}
	D3DXHANDLE GetPowerHandle( void ){
		return *m_hPower;
	}
	D3DXHANDLE GetCameraPosHandle( void ){
		return *m_hCamerapos;
	}
	D3DXHANDLE GetBloomPowerHandle( void ){
		return *m_hBloomPower;
	}
	virtual ~CSkinMeshEffect( void ){};
};

#endif

//EOF