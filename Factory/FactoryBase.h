//============================================================
//!	@file		FactoryBase.h
//!	@brief		���t�@�N�g���[�N���X
//!	@note		LastEdited 2013/11/14
//!	@author		�n�g�������Q�[���w�ȗL�u
//============================================================

// �C���N���[�h�K�[�h
#ifndef _INC_FACTORYBASE_H
#define _INC_FACTORYBASE_H

#include "../Utilities/SmartPointer.h"
#include "../Utilities/ComPointer.h"
#include <list>
#include <map>

using namespace std;

typedef size_t OBJID;		// �I�u�W�F�N�gID
typedef size_t ELEMID;		// �v�fID


//============================================================
//!	@class	IObjectFactory
//!	@brief	�I�u�W�F�N�g��{�C���^�[�t�F�[�X
//============================================================

class IObjectFactory
{
public:
	virtual bool Optimize() = 0;
	// �ێ����Ă���I�u�W�F�N�g�����ׂĉ������
	virtual void ClearAll() = 0;
};

//============================================================
//!	@class	CFactoryManager
//!	@brief	�t�@�N�g���[�}�l�[�W����{�N���X
//============================================================
class CFactoryManager
{

protected:
	
	//============================================================
	// �����o
	//============================================================

	// �i����͎g�p�����t�@�N�g���N���X����
	// �����o�^���Ă����܂��B)
	static list<IObjectFactory*> m_pFactoryList;	//!< @brief �t�@�N�g���I�u�W�F�N�g�̃|�C���^���X�g
	
	//============================================================
	// �R���X�g���N�^&�f�X�g���N�^
	//============================================================
	
	//============================================================
	//!	@brief	�R���X�g���N�^
	//============================================================
	CFactoryManager(){}
	
public:
	virtual ~CFactoryManager()
	{}

	//============================================================
	// ���\�b�h
	//============================================================
public:
	//============================================================
	//!	@fn		Instance
	//!	@brief	�C���X�^���X����&�擾
	//!	@return	�C���X�^���X�{��
	//============================================================
	static CFactoryManager &Instance()
	{
		static CFactoryManager instance;	// �V���O���g���p�^�[��
		return instance;
	}
	//============================================================
	//!	@fn		RegistFactory
	//!	@brief	�Ǘ�����t�@�N�g���I�u�W�F�N�g��o�^
	//!	@param	ptr �Ǘ����ɒu��IObjectFactory�^�̃|�C���^
	//!	@return	���������� true
	//============================================================
	bool RegistFactory( IObjectFactory* ptr )
	{
		m_pFactoryList.push_back( ptr );
		return true;
	}
	
	//============================================================
	//!	@fn		Optimize
	//!	@brief	�e�t�@�N�g���ɍœK�����w������
	//!	@return	���������� true
	//============================================================
	virtual bool Optimize()
	{
		list<IObjectFactory*>::iterator it;
		for( it=m_pFactoryList.begin(); it != m_pFactoryList.end(); ++it )
		{
			(*it)->Optimize();
		}
		return true;
	}
	
	//============================================================
	//!	@fn		ClearAll
	//!	@brief	�S�t�@�N�g���ɃN���A���߂��o��
	//============================================================
	virtual void ClearAll(){
		list<IObjectFactory*>::iterator it;
		for( it = m_pFactoryList.begin(); it != m_pFactoryList.end(); ++it )
		{
			(*it)->ClearAll();
		}
	}
};

#ifdef _MAIN
list<IObjectFactory*> CFactoryManager::m_pFactoryList;	// static�ȃ��X�g���������Ɋm��
#endif

//============================================================
//!	@class	CObjectFactory
//!	@brief	�I�u�W�F�N�g�t�@�N�g����{�N���X
//============================================================
class CObjectFactory : public IObjectFactory
{
protected:
	//============================================================
	//!	@brief	�R���X�g���N�^
	//============================================================
	CObjectFactory()
	{
		CFactoryManager::Instance().RegistFactory( this );
	}
};


//============================================================
//!	@class	CGeneralFactory
//!	@brief	�X�}�[�g�|�C���^�e���v���[�g�V���O���g���t�@�N�g���N���X
//============================================================
template <class T>
class CGeneralFactory : public CObjectFactory
{
	protected:
	typedef pair<ELEMID, SmartPointer<T>>	ELEMPAIR;	// �v�f�y�A
	typedef map<ELEMID, SmartPointer<T>>	ELEMMAP;	// �v�f�}�b�v
	typedef SmartPointer<ELEMMAP>			SPELEMMAP;	// �v�f�}�b�v�̃X�}�[�g�|�C���^
	typedef pair<OBJID, SPELEMMAP >			OBJIDPAIR;	// �I�u�W�F�N�g�y�A
	typedef map<OBJID, SPELEMMAP >			OBJIDMAP;	// �I�u�W�F�N�g�}�b�v
	
	//============================================================
	// �����o
	//============================================================
	OBJIDMAP m_IDMap;		//!< @brief �I�u�W�F�N�g�i�[�}�b�v
	

	//============================================================
	// �R���X�g���N�^&�f�X�g���N�^
	//============================================================
	
	
protected:
	//============================================================
	//!	@brief	�R���X�g���N�^�A�V���O���g���Ȃ̂ŁAprotected�ł���
	//============================================================
	CGeneralFactory(){}
public:
	//============================================================
	//!	@brief	�f�X�g���N�^
	//============================================================
	virtual ~CGeneralFactory(){};
	
	
	//============================================================
	// ���\�b�h
	//============================================================
public:
	//============================================================
	//!	@fn		Instance
	//!	@brief	�V���O���g���C���X�^���X�錾&�擾
	//!	@return	CGeneralFactory<T>�^��Factory�C���X�^���X
	//============================================================
	static CGeneralFactory<T> &Instance()
	{
		static CGeneralFactory<T> inst;
		return inst;
	}
	
	//============================================================
	//!	@fn		Create
	//!	@brief	�I�u�W�F�N�g�𐶐�����A�����ς݂Ȃ�����̃|�C���^��Ԃ�
	//!	@param	id �I�u�W�F�N�g��ID
	//!	@param	elem �v�fID
	//!	@param	spOut �o�͂���鐶���ς݃X�}�[�g�|�C���^
	//!	@return	�I�u�W�F�N�g��ID
	//============================================================
	OBJID Create( OBJID id, size_t elem, SmartPointer<T> &spOut )
	{
		// �I�u�W�F�N�gID�`�F�b�N
		OBJIDMAP::iterator ObjID = m_IDMap.find( id );
		if( ObjID == m_IDMap.end() )
		{
			// �V����ID���v�f�}�b�v�𐶐����ăI�u�W�F�N�g��o�^
			SmartPointer<T> spNewObj( CreateNewObj() );
			ELEMPAIR NewElemPair( elem, spNewObj );
			
			// �V�I�u�W�F�N�gID�Ɨv�f�}�b�v���I�u�W�F�N�g�}�b�v�ɒǉ�
			SPELEMMAP NewElemMap( new map<ELEMID, SmartPointer<T> > );
			NewElemMap->insert( NewElemPair );
			OBJIDPAIR ObjPair( id, NewElemMap );
			m_IDMap.insert( ObjPair );
			spOut = spNewObj;
			return id;
		}
		
		// �v�fID�̃`�F�b�N
		ELEMMAP::iterator it = (*ObjID).second->find(elem);
		if( it == (*ObjID).second->end())
		{
			// �V�K�쐬
			SmartPointer<T> spNewObj( CreateNewObj() );
			ELEMPAIR NewPair( elem, spNewObj );
			(*ObjID).second -> insert( NewPair );
			spOut = spNewObj;
		}
		else
		{
			// �����I�u�W�F�N�g�̎Q�Ɠn��
			spOut = it -> second;
		}
		return id;
	}
	
	//============================================================
	//!	@fn		Optimize
	//!	@brief	�}�b�v���œK�����܂�
	//!	@return	���������� true
	//============================================================
	bool Optimize()
	{
		// �Q�ƃJ�E���g��1�̃I�u�W�F�N�g�͎�菜��
		OBJIDMAP::iterator ObjIt;
		for( ObjIt = m_IDMap.begin(); ObjIt != m_IDMap.end();)
		{
			ELEMMAP::iterator it;
			for( it = ObjIt -> second -> begin(); it != ObjIt ->second->end(); )
			{
				if(*(*it).second.GetRefPtr() != 1){
					it++;
					continue;
				}
				// �폜�Ώ�
				it = ObjIt -> second->erase(it);
			}
			
			// �v�f����0�̃I�u�W�F�N�gID�͎�菜��
			if( ObjIt -> second->size() == 0 )
			{
				ObjIt = m_IDMap.erase( ObjIt );
				continue;
			}
			ObjIt++;
		}
		return true;
	}
	
	//============================================================
	//!	@fn		Size
	//!	@brief	���݂̐����I�u�W�F�N�g�����Z�o
	//!	@return	�I�u�W�F�N�g��
	//============================================================
	virtual size_t Size( void )
	{
		size_t sz = 0;
		OBJIDMAP::iterator ObjIt;
		for( ObjIt = m_IDMap.begin(); ObjIt != m_IDMap.end(); ObjIt++ ){
			sz += ObjIt -> second -> size();
		}
		return sz;
	}
	
	//============================================================
	//!	@fn		ClearAll
	//!	@brief	�S�I�u�W�F�N�g�N���A
	//============================================================
	virtual void ClearAll( void )
	{
		m_IDMap.clear();
	}
	
protected:
	//============================================================
	//!	@fn		CreateNewObj
	//!	@brief	�V�����I�u�W�F�N�g�𐶐�
	//!	@return	�V�����I�u�W�F�N�g�̐��|�C���^
	//============================================================
	virtual T* CreateNewObj(){ return new T; };
};


//============================================================
//!	@class	ICOMObjectFactory
//!	@brief	Com�I�u�W�F�N�g�p�t�@�N�g����{�N���X
//============================================================
template <class T>
class ICOMObjectFactory : public CObjectFactory
{
	typedef pair< ELEMID, T >			ELEMPAIR;	// �v�f�y�A
	typedef map< ELEMID, T >			ELEMMAP;	// �v�f�}�b�v
	typedef SmartPointer<ELEMMAP>		SPELEMMAP;	// �v�f�}�b�v�̃X�}�[�g�|�C���^
	typedef pair<OBJID, SPELEMMAP >		OBJIDPAIR;	// �I�u�W�F�N�g�y�A
	typedef map<OBJID, SPELEMMAP >		OBJIDMAP;	// �I�u�W�F�N�g�}�b�v
	
	OBJIDMAP m_IDMap;	//!< @brief �I�u�W�F�N�g�i�[�}�b�v
	
//============================================================
// �R���X�g���N�^&�f�X�g���N�^
//============================================================
protected:
	//============================================================
	//!	@brief	�R���X�g���N�^
	//============================================================
	ICOMObjectFactory(){}
	
public:
	//============================================================
	//!	@brief	�f�X�g���N�^
	//============================================================
	virtual ~ICOMObjectFactory(){}
	
//============================================================
//!	@fn		Search
//!	@brief	�w��v�f�̌���
//!	@param	id �I�u�W�F�N�gID
//!	@param	elem �v�fID
//!	@return	�v�f�}�b�v�̃C�e���[�^
//============================================================
protected:
	typename ELEMMAP::iterator Search( OBJID id, size_t elem )
	{
		// �I�u�W�F�N�gID�`�F�b�N
		OBJIDMAP::iterator ObjID = m_IDMap.find( id );
		if( ObjID == m_IDMap.end() )
		{
			// �V����ID���v�f�}�b�v�𐶐�
			T cpNewObj;	// �󐶐�
			ELEMPAIR NewElemPair( elem, cpNewObj );
			SPELEMMAP NewElemMap( new ELEMMAP );
			NewElemMap -> insert( NewElemPair );
			
			// �V�I�u�W�F�N�gID�Ɨv�f�}�b�v���I�u�W�F�N�g�}�b�v�ɓo�^
			OBJIDPAIR ObjPair( id, NewElemMap );
			m_IDMap.insert( ObjPair );
			
			// �V�K���������C�e���[�^���o��
			return NewElemMap -> begin();
		}
		
		// �v�fID�̃`�F�b�N
		ELEMMAP::iterator it = (*ObjID).second->find(elem);
		if( it == (*ObjID).second->end())
		{
			// �V�K�쐬
			T cpNewObj;	// �󐶐�
			ELEMPAIR NewPair( elem, cpNewObj );
			return (*ObjID).second -> insert( NewPair ).first;
		}
		
		// �����I�u�W�F�N�g�̎Q�Ɠn��
		return it;
	}
	
	//============================================================
	//!	@fn		Optimize
	//!	@brief	�}�b�v���œK������
	//!	@return	true ����
	//============================================================
	bool Optimize( void )
	{
		// �Q�ƃJ�E���g��1�̃I�u�W�F�N�g�͎�菜��
		OBJIDMAP::iterator ObjIt;
		for( ObjIt = m_IDMap.begin(); ObjIt != m_IDMap.end(); )
		{
			ELEMMAP::iterator it;
			for( it = ObjIt -> second->begin(); it != ObjIt -> second -> end(); )
			{
				if((*it).second.GetPtr()!=NULL)
				{
					// �J�E���g�`�F�b�N
					ULONG cnt = (*it).second->AddRef()-1;
					(*it).second->Release();
					if(cnt != 1)
					{
						it++;
						continue;
					}
				}
				it = ObjIt->second->erase(it);
			}
			
			// �v�f����0�̃I�u�W�F�N�gID�͎�菜��
			if( ObjIt -> second -> size() == 0 )
			{
				ObjIt = m_IDMap.erase( ObjIt );
				continue;
			}
			ObjIt++;
		}
		return true;
	}
	
public:
	//============================================================
	//!	@fn		Size
	//!	@brief	���݂̐����I�u�W�F�N�g�����Z�o
	//!	@return	sz ���݂̃I�u�W�F�N�g��
	//============================================================
	virtual size_t Size( void )
	{
		size_t sz = 0;
		OBJIDMAP::iterator ObjIt;
		for( ObjIt = m_IDMap.begin(); ObjIt != m_IDMap.end(); ObjIt++)
		{
			sz += ObjIt -> second -> size();
		}
		return sz;
	}
	
	//============================================================
	//!	@fn		ClearAll
	//!	@brief	�S�I�u�W�F�N�g�N���A
	//============================================================
	virtual void ClearAll( void )
	{
		m_IDMap.clear();
	}
};
#endif
