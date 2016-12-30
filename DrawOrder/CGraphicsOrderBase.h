//============================================================
//!	@file		CGraphicsOrderBase.h
//!	@brief		�O���t�B�b�N���ߊ�b�N���X
//!	@note		LastEdited 2015/1/31
//!	@author		�n�g�������Q�[���w�ȗL�u
//============================================================

// �C���N���[�h�K�[�h
#ifndef _INC_CGRAPHICSORDERBASE_H
#define _INC_CGRAPHICSORDERBASE_H

#include <d3d9.h>
#include <d3dx9.h>

#include "../Utilities/SmartPointer.h"
#include "../Utilities/ComPointer.h"
#include "../Factory.h"

//============================================================
//!	@class	CGraphicsOrderBase
//!	@brief	�O���t�B�b�N���ߊ�b�N���X
//============================================================
class CGraphicsOrderBase
{
protected:
	enum eGRAPHICS_ORDER {
		eGRAPHICS_BACKGROUND	= 0,
		eGRAPHICS_NON_ALPHA		= 1,
		eGRAPHICS_WITH_ALPHA	= 2,
		eGRAPHICS_SPRITE		= 3,
	};

	size_t	m_priority;
public:
	CGraphicsOrderBase( void ){};
	virtual void Draw( LPDIRECT3DDEVICE9 _device ) = 0;
	virtual ~CGraphicsOrderBase( void ){};
	size_t GetPriority( void ){
		return m_priority;
	}
	bool operator<( CGraphicsOrderBase& obj ){
		if( m_priority < obj.GetPriority() )
			return true;
		return false;
	}
	
	bool operator>( CGraphicsOrderBase& obj ){
		if( m_priority < obj.GetPriority() )
			return true;
		return false;
	}
	
	bool operator==( CGraphicsOrderBase& obj ){
		if( m_priority == obj.GetPriority() )
			return true;
		return false;
	}
};

#endif

//EOF