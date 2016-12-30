//============================================================
//!	@file		CLoadPMX.h
//!	@brief		PMX�t�@�C�����[�h�N���X
//!	@note		LastEdited //
//!	@author		�n�g�������Q�[���w�ȗL�u
//============================================================

// �C���N���[�h�K�[�h
#ifndef _INC_CLOADPMX_H
#define _INC_CLOADPMX_H

#include "PMXStruct.h"

using namespace PmxStruct;
//============================================================
//!	@class	CLoadPMX
//!	@brief	PMX�t�@�C�����[�h�N���X
//============================================================
class CLoadPMX
{
protected:
	PMX_DATA m_pmx_data;
public:
	CLoadPMX( void ){
		m_pmx_data . s_pPmxVertex = NULL;
		m_pmx_data . s_pPmxFace = NULL;
		m_pmx_data . s_pPmxTexture = NULL;
		m_pmx_data . s_pPmxMaterial= NULL;
		m_pmx_data . s_pPmxBone = NULL;
		m_pmx_data . s_pPmxMorph = NULL;
		m_pmx_data . s_pPmxRigidbody = NULL;
		m_pmx_data . s_pPmxJoint = NULL;
	};
	virtual ~CLoadPMX( void ){
		if( m_pmx_data.s_pPmxVertex)
		{
			free(m_pmx_data.s_pPmxVertex);
		}
		if( m_pmx_data.s_pPmxFace)
		{
			free(m_pmx_data.s_pPmxFace);
		}
		if( m_pmx_data.s_pPmxTexture)
		{
			free(m_pmx_data.s_pPmxTexture);
		}
		if( m_pmx_data.s_pPmxMaterial)
		{
			free(m_pmx_data.s_pPmxMaterial);
		}
		if( m_pmx_data.s_pPmxBone)
		{
			free(m_pmx_data.s_pPmxBone);
		}
		
		for( int i = 0 ; ( DWORD )i < m_pmx_data.s_PmxMorphNum ; i ++ )
		{
			switch( m_pmx_data.s_pPmxMorph[ i ].Type )
			{
			case 0 :	// �O���[�v���[�t
				free( m_pmx_data.s_pPmxMorph[ i ].Group );
				break;
			case 1 :	// ���_
				free( m_pmx_data.s_pPmxMorph[ i ].Vertex );
				break;
			case 2 :	// �{�[�����[�t
				free( m_pmx_data.s_pPmxMorph[ i ].Bone );
				break;
			case 3 :	// UV���[�t
			case 4 :	// �ǉ�UV1���[�t
			case 5 :	// �ǉ�UV2���[�t
			case 6 :	// �ǉ�UV3���[�t
			case 7 :	// �ǉ�UV4���[�t
				free( m_pmx_data.s_pPmxMorph[ i ].UV );
				break;
			case 8 :	// �ގ����[�t
				free(m_pmx_data.s_pPmxMorph[ i ].Material);
				break;
			}
		}
		if( m_pmx_data.s_pPmxMorph)
		{
			free(m_pmx_data.s_pPmxMorph);
		}
		if( m_pmx_data.s_pPmxRigidbody)
		{
			free(m_pmx_data.s_pPmxRigidbody);
		}
		if( m_pmx_data.s_pPmxJoint)
		{
			free(m_pmx_data.s_pPmxJoint);
		}
	};

public:
	bool LoadPMXFile( LPCSTR _src );
	PMX_DATA* GetPmxData( void ){
		return &m_pmx_data;
	}
};

#endif