//============================================================
//!	@file		SmartPointer.h
//!	@brief		�X�}�[�g�|�C���^
//!	@note		LastEdited 2013/11/14
//!	@author		�n�g�������Q�[���w�ȗL�u
//============================================================

// �C���N���[�h�K�[�h
#ifndef _INC_SMARTPOINTER_H
#define _INC_SMARTPOINTER_H

#include <Windows.h>
//============================================================
//!	@class	SmartPointer
//!	@brief	�X�}�[�g�|�C���^ (�z�񖢑Ή���)
//============================================================
template <class T>

class SmartPointer
{
private:
	unsigned int *m_pRefCnt;		//!< @brief �Q�ƃJ�E���^�|�C���^
	T** m_ppPtr;						//!< @brief T�^�I�u�W�F�N�g�̃_�u���|�C���^
	static T* m_NullPtr;				//!< @brief NULL�|�C���^�l
	
	//============================================================
	//!	@fn		AddRef
	//!	@brief	�Q�ƃJ�E���^���������[�U������ɐG�邱�Ƃ͂���܂���
	//============================================================
	void AddRef( void ){
		if( m_pRefCnt != 0 ){
			(*m_pRefCnt)++;
		}
	}
	//============================================================
	//!	@fn		Release
	//!	@brief	�Q�ƃJ�E���^�����A���тɍ폜���[�U������ɐG�邱�Ƃ͂���܂���
	//============================================================
	void Release( void ){
		if( m_pRefCnt != 0 && ( *m_pRefCnt == 0 || --(*m_pRefCnt) == 0 ) ) {
			delete *m_ppPtr;
			delete m_ppPtr;
			delete m_pRefCnt;
		}
	}
	
public:
	//============================================================
	//!	@brief	�f�t�H���g�R���X�g���N�^
	//!	@param	�e���v���[�g�^�|�C���^
	//!	@param	�ŏ��ɃZ�b�g����Q�ƃJ�E���^�̒l�B���i�͌Ă΂Ȃ��ł�������
	//============================================================
	explicit SmartPointer( T* ptr = NULL, int add = 0 )
	{
		// �Q�ƃJ�E���^����������
		m_pRefCnt = new unsigned int;
		*m_pRefCnt = add;
		
		m_ppPtr = new T*;	// T�^�̃|�C���^�ϐ��̊i�[�ꏊ�𓮓I�ɐ���
		m_NullPtr = NULL;
		if(ptr)
		{
			*m_ppPtr = ptr;
		}
		else
		{
			*m_ppPtr = m_NullPtr;	// NULL�|�C���^�����Ă���
		}
		AddRef();
	}
	
	//============================================================
	//!	@brief	�R�s�[�R���X�g���N�^
	//============================================================
	SmartPointer( const SmartPointer<T> &src )
	{
		// �|�C���^�R�s�[
		m_pRefCnt = src . m_pRefCnt;
		m_ppPtr = src . m_ppPtr;
		
		// �Q�ƃJ�E���^�𑝉�
		AddRef();
	}
	//============================================================		
	//!	@brief	�A�b�v�L���X�g�t���R�s�[�R���X�g���N�^
	//============================================================
	template<class T2> SmartPointer( SmartPointer<T2> &tmp )
	{
		m_pRefCnt = tmp.GetRefPtr();
		m_ppPtr = (T**)tmp.GetPtrPtr();

		*m_ppPtr = tmp.GetPtr();
		
		
		AddRef();
	}
	
	//============================================================
	//!	@fn		~SmartPointer
	//!	@brief	�f�X�g���N�^
	//============================================================
	virtual ~SmartPointer( void )
	{
		// �Q�ƃJ�E���^������
		Release();
	}
	
public:
	//============================================================
	// ���Z�q�̃I�[�o�[���[�h
	//============================================================

	//============================================================
	//!	@brief	=���Z�q�̃I�[�o�[���[�h
	//============================================================
	SmartPointer& operator =(const SmartPointer<T> &src )
	{
		// �������g�ւ̃R�s�[�͈Ӗ����Ȃ��̂ōs��Ȃ�
		if( *src.m_ppPtr == *m_ppPtr )
			return (*this);
		
		// �����̎Q�ƃJ�E���^��1����
		Release();
		
		// �|�C���^�R�s�[
		m_pRefCnt = src.m_pRefCnt;
		m_ppPtr = src.m_ppPtr;
		
		// �Q�ƃJ�E���^�𑝉�
		AddRef();
		
		return (*this);
	}
	
	//============================================================
	//!	@brief	=������Z�q(�����I�A�b�v�L���X�g)
	//============================================================
	template<class T2> SmartPointer<T>& operator =( SmartPointer<T2> &src )
	{
		if(src.GetPtr()==*m_ppPtr)
			return (*this);
		
		Release();
		
		// ����̃|�C���^���R�s�[
		m_pRefCnt = src.GetRefPtr();
		m_ppPtr = (T**)src . GetPtrPtr();
		
		// �^�`�F�b�N�R�s�[
		*m_ppPtr = src.GetPtr();
		
		// �V�����������g�̎Q�ƃJ�E���^�𑝉�
		AddRef();
		
		return (*this);
	}

	//============================================================
	//!	@brief	=������Z�q(NULL����ɂ�郊�Z�b�g)
	//============================================================
	SmartPointer<T>& operator =(const int nullval)
	{
		// �����͂���Ȑl�ɂȂ��Ă��܂��̂ŁA�Q�ƃJ�E���^��1����
		Release();
		
		// �V�K�Ɏ������g�����
		m_pRefCnt = new unsigned int(1);
		m_ppPtr = new T*;
		*m_ppPtr = m_NullPtr;
		
		return (*this);
	}
	//============================================================
	//!	@brief	->���Z�q�̃I�[�o�[���[�h
	//!	@return	�ێ����Ă���e���v���[�g�ϐ��̃|�C���^�A�h���X
	//============================================================
	T* operator ->()
	{
		return *m_ppPtr;
	}
	//============================================================
	//!	@brief	*���Z�q�̃I�[�o�[���[�h
	//!	@return	���[�h�I�����[�ȃe���v���[�g�^�|�C���^�̒��g
	//============================================================
	T& operator *()
	{
		return **m_ppPtr;
	}
	
	bool operator ==( T* val){
		if(*m_ppPtr == val)
			return true;
		return false;
	}
	
public:

	//============================================================
	// ���\�b�h
	//============================================================

	//============================================================
	//!	@fn		SetPtr
	//!	@brief	�|�C���^�ڏ�
	//!	@param	�e���v���[�g�^�|�C���^
	//============================================================
	void SetPtr( T* src = 0 )
	{
		// �Q�ƃJ�E���^�����炵����ɍď�����
		Release();
		m_pRefCnt = new unsigned int;
		*m_pRefCnt = 0;
		m_ppPtr = new T*;	// T�^�̃|�C���^�ϐ��̊i�[�ꏊ�𓮓I�ɐ���
		m_NullPtr = NULL;
		if(src)
		{
			*m_ppPtr = src;
		}
		else
		{
			*m_ppPtr = m_NullPtr;	// NULL�|�C���^�����Ă���
		}
		AddRef();
	}

	//============================================================
	//!	@fn		GetRefPtr
	//!	@brief	�Q�ƃJ�E���^�A�h���X�擾
	//!	@return	�Q�ƃJ�E���^�A�h���X
	//============================================================
	unsigned int* GetRefPtr( void )
	{
		return m_pRefCnt;
	}
	//============================================================
	//!	@fn		GetPtr
	//!	@brief	�|�C���^�擾
	//!	@return	�ێ����Ă���e���v���[�g�^�|�C���^
	//============================================================
	T* GetPtr( void )
	{
		return *m_ppPtr;
	}
	
	//============================================================
	//!	@fn		GetPtrPtr
	//!	@brief	�|�C���^�擾
	//!	@return	�ێ����Ă���e���v���[�g�^�|�C���^
	//============================================================
	T** GetPtrPtr( void )
	{
		return m_ppPtr;
	}

	//============================================================
	//!	@fn		DownCast
	//!	@brief	�_�E���L���X�g
	//!	@param	�_�C�i�~�b�N�L���X�g�𗘗p�����_�E���L���X�g
	//!	@return	����������A
	//============================================================
	template <class T2>
	bool DownCast( SmartPointer<T2> &src )
	{
		// �����̃X�}�[�g�|�C���^�����|�C���^�������̓o�^���Ă���
		// �|�C���^�Ƀ_�E���L���X�g�\�ȏꍇ�̓I�u�W�F�N�g��Ԃ�
		T* castPtr = dynamic_cast<T*>(src.GetPtr());
		if( castPtr != NULL ){
			// �_�E���L���X�g�����A�����͈Ⴄ�l�ɂȂ�̂Ŋ����̎Q�ƃJ�E���^��1����
			Release();
			
			// �V�����|�C���^�ƎQ�ƃJ�E���^�����L
			m_ppPtr = (T**)src.GetPtrPtr();
			*m_ppPtr = castPtr;
			m_pRefCnt = src . GetRefPtr();
			
			// �Q�ƃJ�E���^����
			AddRef();
			return true;
		}
		
		// �L���X�g���s
		return false;
	}
	//============================================================
	//!	@fn		SwapPtr
	//!	@brief	�|�C���^�̃X���b�v
	//!	@param	�X���b�v�����������^�̃X�}�[�g�|�C���^
	//============================================================
	void SwapPtr( SmartPointer<T> &src )
	{
		T* pTmp = src.GetPtr();
		*src.m_ppPtr = *m_ppPtr;	// �|�C���^�̌���
		*m_ppPtr = pTmp;
	}
};

template <class T>
T* SmartPointer<T>::m_NullPtr = NULL;

#define sp SmartPointer
#endif

//EOF