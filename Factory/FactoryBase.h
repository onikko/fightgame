//============================================================
//!	@file		FactoryBase.h
//!	@brief		基底ファクトリークラス
//!	@note		LastEdited 2013/11/14
//!	@author		ＯＨ教務部ゲーム学科有志
//============================================================

// インクルードガード
#ifndef _INC_FACTORYBASE_H
#define _INC_FACTORYBASE_H

#include "../Utilities/SmartPointer.h"
#include "../Utilities/ComPointer.h"
#include <list>
#include <map>

using namespace std;

typedef size_t OBJID;		// オブジェクトID
typedef size_t ELEMID;		// 要素ID


//============================================================
//!	@class	IObjectFactory
//!	@brief	オブジェクト基本インターフェース
//============================================================

class IObjectFactory
{
public:
	virtual bool Optimize() = 0;
	// 保持しているオブジェクトをすべて解放する
	virtual void ClearAll() = 0;
};

//============================================================
//!	@class	CFactoryManager
//!	@brief	ファクトリーマネージャ基本クラス
//============================================================
class CFactoryManager
{

protected:
	
	//============================================================
	// メンバ
	//============================================================

	// （これは使用したファクトリクラス内で
	// 自動登録していきます。)
	static list<IObjectFactory*> m_pFactoryList;	//!< @brief ファクトリオブジェクトのポインタリスト
	
	//============================================================
	// コンストラクタ&デストラクタ
	//============================================================
	
	//============================================================
	//!	@brief	コンストラクタ
	//============================================================
	CFactoryManager(){}
	
public:
	virtual ~CFactoryManager()
	{}

	//============================================================
	// メソッド
	//============================================================
public:
	//============================================================
	//!	@fn		Instance
	//!	@brief	インスタンス生成&取得
	//!	@return	インスタンス本体
	//============================================================
	static CFactoryManager &Instance()
	{
		static CFactoryManager instance;	// シングルトンパターン
		return instance;
	}
	//============================================================
	//!	@fn		RegistFactory
	//!	@brief	管理するファクトリオブジェクトを登録
	//!	@param	ptr 管理下に置くIObjectFactory型のポインタ
	//!	@return	成功したら true
	//============================================================
	bool RegistFactory( IObjectFactory* ptr )
	{
		m_pFactoryList.push_back( ptr );
		return true;
	}
	
	//============================================================
	//!	@fn		Optimize
	//!	@brief	各ファクトリに最適化を指示する
	//!	@return	成功したら true
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
	//!	@brief	全ファクトリにクリア命令を出す
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
list<IObjectFactory*> CFactoryManager::m_pFactoryList;	// staticなリストをメモリに確保
#endif

//============================================================
//!	@class	CObjectFactory
//!	@brief	オブジェクトファクトリ基本クラス
//============================================================
class CObjectFactory : public IObjectFactory
{
protected:
	//============================================================
	//!	@brief	コンストラクタ
	//============================================================
	CObjectFactory()
	{
		CFactoryManager::Instance().RegistFactory( this );
	}
};


//============================================================
//!	@class	CGeneralFactory
//!	@brief	スマートポインタテンプレートシングルトンファクトリクラス
//============================================================
template <class T>
class CGeneralFactory : public CObjectFactory
{
	protected:
	typedef pair<ELEMID, SmartPointer<T>>	ELEMPAIR;	// 要素ペア
	typedef map<ELEMID, SmartPointer<T>>	ELEMMAP;	// 要素マップ
	typedef SmartPointer<ELEMMAP>			SPELEMMAP;	// 要素マップのスマートポインタ
	typedef pair<OBJID, SPELEMMAP >			OBJIDPAIR;	// オブジェクトペア
	typedef map<OBJID, SPELEMMAP >			OBJIDMAP;	// オブジェクトマップ
	
	//============================================================
	// メンバ
	//============================================================
	OBJIDMAP m_IDMap;		//!< @brief オブジェクト格納マップ
	

	//============================================================
	// コンストラクタ&デストラクタ
	//============================================================
	
	
protected:
	//============================================================
	//!	@brief	コンストラクタ、シングルトンなので、protectedである
	//============================================================
	CGeneralFactory(){}
public:
	//============================================================
	//!	@brief	デストラクタ
	//============================================================
	virtual ~CGeneralFactory(){};
	
	
	//============================================================
	// メソッド
	//============================================================
public:
	//============================================================
	//!	@fn		Instance
	//!	@brief	シングルトンインスタンス宣言&取得
	//!	@return	CGeneralFactory<T>型のFactoryインスタンス
	//============================================================
	static CGeneralFactory<T> &Instance()
	{
		static CGeneralFactory<T> inst;
		return inst;
	}
	
	//============================================================
	//!	@fn		Create
	//!	@brief	オブジェクトを生成する、生成済みなら既存のポインタを返す
	//!	@param	id オブジェクトのID
	//!	@param	elem 要素ID
	//!	@param	spOut 出力される生成済みスマートポインタ
	//!	@return	オブジェクトのID
	//============================================================
	OBJID Create( OBJID id, size_t elem, SmartPointer<T> &spOut )
	{
		// オブジェクトIDチェック
		OBJIDMAP::iterator ObjID = m_IDMap.find( id );
		if( ObjID == m_IDMap.end() )
		{
			// 新しいID内要素マップを生成してオブジェクトを登録
			SmartPointer<T> spNewObj( CreateNewObj() );
			ELEMPAIR NewElemPair( elem, spNewObj );
			
			// 新オブジェクトIDと要素マップをオブジェクトマップに追加
			SPELEMMAP NewElemMap( new map<ELEMID, SmartPointer<T> > );
			NewElemMap->insert( NewElemPair );
			OBJIDPAIR ObjPair( id, NewElemMap );
			m_IDMap.insert( ObjPair );
			spOut = spNewObj;
			return id;
		}
		
		// 要素IDのチェック
		ELEMMAP::iterator it = (*ObjID).second->find(elem);
		if( it == (*ObjID).second->end())
		{
			// 新規作成
			SmartPointer<T> spNewObj( CreateNewObj() );
			ELEMPAIR NewPair( elem, spNewObj );
			(*ObjID).second -> insert( NewPair );
			spOut = spNewObj;
		}
		else
		{
			// 既存オブジェクトの参照渡し
			spOut = it -> second;
		}
		return id;
	}
	
	//============================================================
	//!	@fn		Optimize
	//!	@brief	マップを最適化します
	//!	@return	成功したら true
	//============================================================
	bool Optimize()
	{
		// 参照カウントが1のオブジェクトは取り除く
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
				// 削除対象
				it = ObjIt -> second->erase(it);
			}
			
			// 要素数が0のオブジェクトIDは取り除く
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
	//!	@brief	現在の生成オブジェクト数を算出
	//!	@return	オブジェクト数
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
	//!	@brief	全オブジェクトクリア
	//============================================================
	virtual void ClearAll( void )
	{
		m_IDMap.clear();
	}
	
protected:
	//============================================================
	//!	@fn		CreateNewObj
	//!	@brief	新しいオブジェクトを生成
	//!	@return	新しいオブジェクトの生ポインタ
	//============================================================
	virtual T* CreateNewObj(){ return new T; };
};


//============================================================
//!	@class	ICOMObjectFactory
//!	@brief	Comオブジェクト用ファクトリ基本クラス
//============================================================
template <class T>
class ICOMObjectFactory : public CObjectFactory
{
	typedef pair< ELEMID, T >			ELEMPAIR;	// 要素ペア
	typedef map< ELEMID, T >			ELEMMAP;	// 要素マップ
	typedef SmartPointer<ELEMMAP>		SPELEMMAP;	// 要素マップのスマートポインタ
	typedef pair<OBJID, SPELEMMAP >		OBJIDPAIR;	// オブジェクトペア
	typedef map<OBJID, SPELEMMAP >		OBJIDMAP;	// オブジェクトマップ
	
	OBJIDMAP m_IDMap;	//!< @brief オブジェクト格納マップ
	
//============================================================
// コンストラクタ&デストラクタ
//============================================================
protected:
	//============================================================
	//!	@brief	コンストラクタ
	//============================================================
	ICOMObjectFactory(){}
	
public:
	//============================================================
	//!	@brief	デストラクタ
	//============================================================
	virtual ~ICOMObjectFactory(){}
	
//============================================================
//!	@fn		Search
//!	@brief	指定要素の検索
//!	@param	id オブジェクトID
//!	@param	elem 要素ID
//!	@return	要素マップのイテレータ
//============================================================
protected:
	typename ELEMMAP::iterator Search( OBJID id, size_t elem )
	{
		// オブジェクトIDチェック
		OBJIDMAP::iterator ObjID = m_IDMap.find( id );
		if( ObjID == m_IDMap.end() )
		{
			// 新しいID内要素マップを生成
			T cpNewObj;	// 空生成
			ELEMPAIR NewElemPair( elem, cpNewObj );
			SPELEMMAP NewElemMap( new ELEMMAP );
			NewElemMap -> insert( NewElemPair );
			
			// 新オブジェクトIDと要素マップをオブジェクトマップに登録
			OBJIDPAIR ObjPair( id, NewElemMap );
			m_IDMap.insert( ObjPair );
			
			// 新規生成したイテレータを出力
			return NewElemMap -> begin();
		}
		
		// 要素IDのチェック
		ELEMMAP::iterator it = (*ObjID).second->find(elem);
		if( it == (*ObjID).second->end())
		{
			// 新規作成
			T cpNewObj;	// 空生成
			ELEMPAIR NewPair( elem, cpNewObj );
			return (*ObjID).second -> insert( NewPair ).first;
		}
		
		// 既存オブジェクトの参照渡し
		return it;
	}
	
	//============================================================
	//!	@fn		Optimize
	//!	@brief	マップを最適化する
	//!	@return	true 成功
	//============================================================
	bool Optimize( void )
	{
		// 参照カウントが1のオブジェクトは取り除く
		OBJIDMAP::iterator ObjIt;
		for( ObjIt = m_IDMap.begin(); ObjIt != m_IDMap.end(); )
		{
			ELEMMAP::iterator it;
			for( it = ObjIt -> second->begin(); it != ObjIt -> second -> end(); )
			{
				if((*it).second.GetPtr()!=NULL)
				{
					// カウントチェック
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
			
			// 要素数が0のオブジェクトIDは取り除く
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
	//!	@brief	現在の生成オブジェクト数を算出
	//!	@return	sz 現在のオブジェクト数
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
	//!	@brief	全オブジェクトクリア
	//============================================================
	virtual void ClearAll( void )
	{
		m_IDMap.clear();
	}
};
#endif
