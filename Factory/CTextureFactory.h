//============================================================
//!	@file		CTextureFactory.h
//!	@brief		�e�N�X�`�������A�Ǘ��p�t�@�N�g���[�N���X
//!	@note		LastEdited 2013/11/28
//!	@author		�n�g�������Q�[���w�ȗL�u
//============================================================

// �C���N���[�h�K�[�h
#ifndef _INC_CTEXTUREFACTORY_H
#define _INC_CTEXTUREFACTORY_H

#include "FactoryBase.h"
#include "../Utilities/ComPointer.h"
#include "../Utilities/GameObj.h"

//============================================================
//!	@class	CTextureFactory
//!	@brief	�G�t�F�N�g�t�@�C�������Ǘ��p�N���X
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
			// ����
			cpTexture = it->second;
			return true;
		}

		// �V�K�쐬
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