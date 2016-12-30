//============================================================
//!	@file		CEffect.h
//!	@brief		
//!	@note		LastEdited //
//!	@author		ＯＨ教務部ゲーム学科有志
//============================================================

// インクルードガード
#ifndef _INC_CEFFECT_H
#define _INC_CEFFECT_H

#include "Factory.h"

//============================================================
//!	@class	CEffect
//!	@brief	エフェクト管理用基底クラス
//============================================================
class CEffect
{
public:
	CEffect( void ){
	};
	virtual bool LoadEffect( LPCSTR effectpath ){
	return true;
	};
	virtual ~CEffect( void ){
	};

public:
	enum eSHADER_TYPE
	{
		eMESH = 0,
		eSKIN_MESH = 1,
		eSPRITE = 2,
		ePOINT = 3,
		ePOLYLINE = 4,
	};

	enum eHANDLE_NAME
	{
		eHANDLE_TECH = 0,
		eHANDLE_WORLD = 1,
		eHANDLE_VIEW = 2,
		eHANDLE_PROJ = 3,
		eHANDLE_AMBIENT = 4,
		eHANDLE_DIFFUSE = 5,
		eHANDLE_LIGHTDIR = 6,
		eHANDLE_TEXTURE = 7,
		eHANDLE_SPECULAR = 8,
		eHANDLE_POWER = 9,
		eHANDLE_CAMERAPOS = 10,
		eHANDLE_BLOOMPOWER = 11,
		eHANDLE_WVP = 12,
	};
};

#endif

//EOF