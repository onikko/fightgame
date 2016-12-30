//============================================================
//!	@file		CGraphicsDrawList
//!	@brief		�O���t�B�b�N�`�惊�X�g
//!	@note		LastEdited 2015/01/31
//!	@author		�n�g�������Q�[���w�ȗL�u
//============================================================

// �C���N���[�h�K�[�h
#ifndef _INC_CGRAPHICSDRAWLIST_H
#define _INC_CGRAPHICSDRAWLIST_H

#include <vector>
#include <algorithm>

//#include "../Task/CTaskEx.h"
#include "CGraphicsOrderBase.h"

typedef SmartPointer< CGraphicsOrderBase> SP_GR_ORDER;

using namespace std;

class LessOrder{
public:
	bool operator()( SP_GR_ORDER& riLeft, SP_GR_ORDER& riRIght ) const {
		return riLeft->GetPriority() < riRIght->GetPriority();
	}
};
//============================================================
//!	@class	CGraphicsDrawList
//!	@brief	�O���t�B�b�N�`�惊�X�g
//============================================================
class CGraphicsDrawList
{
//============================================================
// �����o
//============================================================
private:
	static vector< SP_GR_ORDER > m_order_list;

//============================================================
// ���\�b�h
//============================================================
private:
	CGraphicsDrawList( void ){
	};
public:
	static CGraphicsDrawList& Instance( void ){
		static CGraphicsDrawList inst;
		return inst;
	}
	
	bool SetOrder( SP_GR_ORDER sp_order ){
		m_order_list . push_back( sp_order );
		return true;
	}
	
	bool StartDraw( LPDIRECT3DDEVICE9 _device ){
		sort(m_order_list.begin(), m_order_list.end(),LessOrder());
		
		for_each( m_order_list.begin(), m_order_list.end(), 
		[_device]( SP_GR_ORDER order) -> void{ order->Draw( _device ); } );

		m_order_list.clear();
		
		return true;
	}
	
	SP_GR_ORDER getaddress( void )
	{
		return m_order_list[1];
	}

	virtual ~CGraphicsDrawList( void ){
		m_order_list . clear();
	};
};

#ifdef _MAIN
vector< SP_GR_ORDER > CGraphicsDrawList::m_order_list;
#endif

#endif

//EOF