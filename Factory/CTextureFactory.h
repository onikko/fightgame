//============================================================
//!	@file		CTextureFactory.h
//!	@brief		テクスチャ生成、管理用ファクトリークラス
//!	@note		LastEdited 2013/11/28
//!	@author		ＯＨ教務部ゲーム学科有志
//============================================================

// インクルードガード
#ifndef _INC_CTEXTUREFACTORY_H
#define _INC_CTEXTUREFACTORY_H

#include "FactoryBase.h"
#include "../Utilities/ComPointer.h"
#include "../Utilities/GameObj.h"

//============================================================
//!	@class	CTextureFactory
//!	@brief	エフェクトファイル生成管理用クラス
//============================================================
class CTextureFactory : public ICOMObjectFactory<cp<IDirect3DTexture9> >
{
	typedef cp<IDirect3DTexture9> ComTexture;
protected:
	CTextureFactory( void ){};
public:
	virtual ~CTextureFactory( void ){};


	static CTextureFactory &Instance( void )
	{
		static CTextureFactory instance;
		return instance;
	}
	bool CreateTexture( OBJID objid, ELEMID elemid, LPDIRECT3DDEVICE9 pDevice, LPCTSTR pSrcFile, ComTexture &cpTexture )
	{
		ELEMMAP::iterator it = Search( objid, elemid );
		if(it->second.GetPtr()!=NULL){
			// 既存
			cpTexture = it->second;
			return true;
		}

		// 新規作成
		HRESULT res = D3DXCreateTextureFromFile( pDevice, pSrcFile,it->second.ToCreater());
		if( FAILED(res)){
			return false;
		}
		cpTexture = it->second;
		return true;
	}
};

#endif

//EOF