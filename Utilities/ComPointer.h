//============================================================
//!	@file		ComPointer.h
//!	@brief		Com�e���v���[�g�|�C���^�[�N���X
//!	@note		LastEdited 2013/11/18
//!	@author		�n�g�������Q�[���w�ȗL�u
//============================================================

// �C���N���[�h�K�[�h
#ifndef _INC_COMPOINTER_H
#define _INC_COMPOINTER_H

#include <Windows.h>

//============================================================
//!	@class	ComPointer
//!	@brief	Com�|�C���^�[�N���X
//============================================================
template <class T>
class ComPointer
{
//============================================================
// �����o
//============================================================
	T* m_pInterface;	//!< @brief �ێ����Ă���Com�|�C���^�[�A�h���X
	
//============================================================
// �V�[�N���b�g���\�b�h
//============================================================
protected:
	// �Q�ƃJ�E���^����
	void AddRef(T* pInterface){
		pInterface -> AddRef();
	}
	// �Q�ƃJ�E���^����
	void Release( T* pInterface ){
		m_pInterface -> Release();
	};
	
//============================================================
// �R���X�g���N�^&�f�X�g���N�^
//============================================================
public:
	//============================================================
	//!	@fn		ComPointer
	//!	@brief	�f�t�H���g�R���X�g���N�^
	//!	@param	pInterface �i�[����C���^�[�t�F�[�X
	//!	@param	add ���Z�t���O
	//============================================================
	explicit ComPointer( T* pInterface = NULL, bool add = false )
	{
		if( pInterface && add ){
			AddRef(pInterface );
		}
		m_pInterface = pInterface;
	}
	
	//============================================================
	//!	@fn		ComPointer
	//!	@brief	�R�s�[�R���X�g���N�^
	//!	@param	src �R�s�[����Com�|�C���^�[
	//============================================================
	ComPointer( const ComPointer& src )
	{
		// �R�s�[���̎Q�ƃJ�E���^��1���₷
		if( src.m_pInterface ){
			AddRef(src.m_pInterface);
		}
		
		// �R�s�[
		m_pInterface = src.m_pInterface;
	}
	
	//============================================================
	//!	@fn		~ComPointer
	//!	@brief	�f�X�g���N�^
	//============================================================
	virtual ~ComPointer( void )
	{
		if(m_pInterface){
			Release( m_pInterface );
		}
	}
	
//============================================================
// ���Z�q�̃I�[�o�[���[�h
//============================================================
public:
	//============================================================
	//!	@fn		operator =
	//!	@param	src �R�s�[��Com�|�C���^
	//!	@brief	=���Z�q�̃I�[�o�[���[�h
	//============================================================
	ComPointer& operator =(const ComPointer& src ){
		// �����̃C���^�[�t�F�[�X�̎Q�ƃJ�E���^��1���炷
		if( m_pInterface ){
			Release( m_pInterface );
		}
		
		// �R�s�[���̎Q�ƃJ�E���^��1���₷
		if(src.m_pInterface){
			AddRef(src.m_pInterface);
		}
		
		// �R�s�[
		m_pInterface = src.m_pInterface;
		
		return (*this);
	}
	
	//============================================================
	//!	@brief	=���Z�q�̃I�[�o�[���[�h(�C���^�[�t�F�C�X�o�^)
	//!	@param	pInterface �o�^����C���^�[�t�F�[�X�A�h���X
	//============================================================
	void operator =( T* pInterface )
	{
		// �����̃C���^�[�t�F�C�X�̎Q�ƃJ�E���^��1���炷
		if(m_pInterface)
		{
			Release(m_pInterface);
		}
		// �R�s�[
		m_pInterface = pInterface;
	}
	
//============================================================
// ���\�b�h
//============================================================
public:
	//============================================================
	//!	@fn		GerPtr
	//!	@brief	�|�C���^�擾
	//!	@return	�����Ă���C���^�[�t�F�[�X�|�C���^
	//============================================================
	T* GetPtr( void )
	{
		return m_pInterface;
	}
	
	//============================================================
	//!	@fn		GetPtrPtr
	//!	@brief	�|�C���^�̃A�h���X���擾
	//!	@return	�������Ă���C���^�[�t�F�[�X�̃|�C���^�̃|�C���^
	//============================================================
	T** GetPtrPtr( void )
	{
		return &m_pInterface;
	}
	
	T* operator ->( void ){ return m_pInterface; }
	//============================================================
	//!	@fn		ToCreater
	//!	@brief	�C���^�[�t�F�[�X�����֐��֓n����p�֐�
	//!	@return	�������Ă���C���^�[�t�F�[�X�̃|�C���^�̃|�C���^
	//============================================================
	T** ToCreater( void )
	{
		if( m_pInterface )
		{
			m_pInterface -> Release();
		}
		
		return &m_pInterface;
	}
};

#define cp ComPointer
#endif

//EOF