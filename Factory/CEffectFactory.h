//============================================================
//!	@file		CEffectFactory.h
//!	@brief		エフェクト生成、管理用ファクトリークラス
//!	@note		LastEdited 2013/11/28
//!	@author		ＯＨ教務部ゲーム学科有志
//============================================================

// インクルードガード
#ifndef _INC_CEFFECTFACTORY_H
#define _INC_CEFFECTFACTORY_H

#include "FactoryBase.h"
#include "../Utilities/ComPointer.h"

#include "../Utilities/GameObj.h"

//============================================================
//!	@class	CEffectFactory
//!	@brief	エフェクトファイル生成管理用クラス
//============================================================
class CEffectFactory : public ICOMObjectFactory<cp<ID3DXEffect> >
{
	typedef cp<ID3DXEffect> ComEffect;
public:
	enum EFFECT_NAME{
		BASIC_PHONG,
	};
	enum EFFECT_TYPE{
		MESH,
		SKIN_MESH,
	};
private:
protected:
	CEffectFactory( void ){};
public:
	virtual ~CEffectFactory( void ){};


	static CEffectFactory &Instance( void )
	{
		static CEffectFactory instance;
		return instance;
	}
	bool CreateEffect( OBJID objid, ELEMID elemid, LPDIRECT3DDEVICE9 pDevice, LPCTSTR pSrcFile, DWORD flag, ComEffect &cpEffect )
	{
		LPD3DXBUFFER	errors;

		ELEMMAP::iterator it = Search( objid, elemid );
		if(it->second.GetPtr()!=NULL){
			// 既存
			cpEffect = it->second;
			return true;
		}

		// 新規作成
		HRESULT res = D3DXCreateEffectFromFile( pDevice, pSrcFile,0,0,flag,0, it->second.ToCreater(), &errors );
		if( FAILED(res)){
			return false;
		}
		cpEffect = it->second;
		return true;
	}
};

#endif

//EOF