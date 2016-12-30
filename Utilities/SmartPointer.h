//============================================================
//!	@file		SmartPointer.h
//!	@brief		スマートポインタ
//!	@note		LastEdited 2013/11/14
//!	@author		ＯＨ教務部ゲーム学科有志
//============================================================

// インクルードガード
#ifndef _INC_SMARTPOINTER_H
#define _INC_SMARTPOINTER_H

#include <Windows.h>
//============================================================
//!	@class	SmartPointer
//!	@brief	スマートポインタ (配列未対応版)
//============================================================
template <class T>

class SmartPointer
{
private:
	unsigned int *m_pRefCnt;		//!< @brief 参照カウンタポインタ
	T** m_ppPtr;						//!< @brief T型オブジェクトのダブルポインタ
	static T* m_NullPtr;				//!< @brief NULLポインタ値
	
	//============================================================
	//!	@fn		AddRef
	//!	@brief	参照カウンタ増幅※ユーザがこれに触ることはありません
	//============================================================
	void AddRef( void ){
		if( m_pRefCnt != 0 ){
			(*m_pRefCnt)++;
		}
	}
	//============================================================
	//!	@fn		Release
	//!	@brief	参照カウンタ減少、並びに削除ユーザがこれに触ることはありません
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
	//!	@brief	デフォルトコンストラクタ
	//!	@param	テンプレート型ポインタ
	//!	@param	最初にセットする参照カウンタの値。普段は呼ばないでください
	//============================================================
	explicit SmartPointer( T* ptr = NULL, int add = 0 )
	{
		// 参照カウンタ生成初期化
		m_pRefCnt = new unsigned int;
		*m_pRefCnt = add;
		
		m_ppPtr = new T*;	// T型のポインタ変数の格納場所を動的に生成
		m_NullPtr = NULL;
		if(ptr)
		{
			*m_ppPtr = ptr;
		}
		else
		{
			*m_ppPtr = m_NullPtr;	// NULLポインタを入れておく
		}
		AddRef();
	}
	
	//============================================================
	//!	@brief	コピーコンストラクタ
	//============================================================
	SmartPointer( const SmartPointer<T> &src )
	{
		// ポインタコピー
		m_pRefCnt = src . m_pRefCnt;
		m_ppPtr = src . m_ppPtr;
		
		// 参照カウンタを増加
		AddRef();
	}
	//============================================================		
	//!	@brief	アップキャスト付きコピーコンストラクタ
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
	//!	@brief	デストラクタ
	//============================================================
	virtual ~SmartPointer( void )
	{
		// 参照カウンタを減少
		Release();
	}
	
public:
	//============================================================
	// 演算子のオーバーロード
	//============================================================

	//============================================================
	//!	@brief	=演算子のオーバーロード
	//============================================================
	SmartPointer& operator =(const SmartPointer<T> &src )
	{
		// 自分自身へのコピーは意味がないので行わない
		if( *src.m_ppPtr == *m_ppPtr )
			return (*this);
		
		// 自分の参照カウンタを1つ減少
		Release();
		
		// ポインタコピー
		m_pRefCnt = src.m_pRefCnt;
		m_ppPtr = src.m_ppPtr;
		
		// 参照カウンタを増加
		AddRef();
		
		return (*this);
	}
	
	//============================================================
	//!	@brief	=代入演算子(明示的アップキャスト)
	//============================================================
	template<class T2> SmartPointer<T>& operator =( SmartPointer<T2> &src )
	{
		if(src.GetPtr()==*m_ppPtr)
			return (*this);
		
		Release();
		
		// 相手のポインタをコピー
		m_pRefCnt = src.GetRefPtr();
		m_ppPtr = (T**)src . GetPtrPtr();
		
		// 型チェックコピー
		*m_ppPtr = src.GetPtr();
		
		// 新しい自分自身の参照カウンタを増加
		AddRef();
		
		return (*this);
	}

	//============================================================
	//!	@brief	=代入演算子(NULL代入によるリセット)
	//============================================================
	SmartPointer<T>& operator =(const int nullval)
	{
		// 自分はからな人になってしまうので、参照カウンタを1つ減少
		Release();
		
		// 新規に自分自身を作る
		m_pRefCnt = new unsigned int(1);
		m_ppPtr = new T*;
		*m_ppPtr = m_NullPtr;
		
		return (*this);
	}
	//============================================================
	//!	@brief	->演算子のオーバーロード
	//!	@return	保持しているテンプレート変数のポインタアドレス
	//============================================================
	T* operator ->()
	{
		return *m_ppPtr;
	}
	//============================================================
	//!	@brief	*演算子のオーバーロード
	//!	@return	リードオンリーなテンプレート型ポインタの中身
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
	// メソッド
	//============================================================

	//============================================================
	//!	@fn		SetPtr
	//!	@brief	ポインタ移譲
	//!	@param	テンプレート型ポインタ
	//============================================================
	void SetPtr( T* src = 0 )
	{
		// 参照カウンタを減らした後に再初期化
		Release();
		m_pRefCnt = new unsigned int;
		*m_pRefCnt = 0;
		m_ppPtr = new T*;	// T型のポインタ変数の格納場所を動的に生成
		m_NullPtr = NULL;
		if(src)
		{
			*m_ppPtr = src;
		}
		else
		{
			*m_ppPtr = m_NullPtr;	// NULLポインタを入れておく
		}
		AddRef();
	}

	//============================================================
	//!	@fn		GetRefPtr
	//!	@brief	参照カウンタアドレス取得
	//!	@return	参照カウンタアドレス
	//============================================================
	unsigned int* GetRefPtr( void )
	{
		return m_pRefCnt;
	}
	//============================================================
	//!	@fn		GetPtr
	//!	@brief	ポインタ取得
	//!	@return	保持しているテンプレート型ポインタ
	//============================================================
	T* GetPtr( void )
	{
		return *m_ppPtr;
	}
	
	//============================================================
	//!	@fn		GetPtrPtr
	//!	@brief	ポインタ取得
	//!	@return	保持しているテンプレート型ポインタ
	//============================================================
	T** GetPtrPtr( void )
	{
		return m_ppPtr;
	}

	//============================================================
	//!	@fn		DownCast
	//!	@brief	ダウンキャスト
	//!	@param	ダイナミックキャストを利用したダウンキャスト
	//!	@return	成功したら、
	//============================================================
	template <class T2>
	bool DownCast( SmartPointer<T2> &src )
	{
		// 引数のスマートポインタが持つポインタが自分の登録している
		// ポインタにダウンキャスト可能な場合はオブジェクトを返す
		T* castPtr = dynamic_cast<T*>(src.GetPtr());
		if( castPtr != NULL ){
			// ダウンキャスト成功、自分は違う人になるので既存の参照カウンタを1つ減少
			Release();
			
			// 新しいポインタと参照カウンタを共有
			m_ppPtr = (T**)src.GetPtrPtr();
			*m_ppPtr = castPtr;
			m_pRefCnt = src . GetRefPtr();
			
			// 参照カウンタ増加
			AddRef();
			return true;
		}
		
		// キャスト失敗
		return false;
	}
	//============================================================
	//!	@fn		SwapPtr
	//!	@brief	ポインタのスワップ
	//!	@param	スワップしたい同じ型のスマートポインタ
	//============================================================
	void SwapPtr( SmartPointer<T> &src )
	{
		T* pTmp = src.GetPtr();
		*src.m_ppPtr = *m_ppPtr;	// ポインタの交換
		*m_ppPtr = pTmp;
	}
};

template <class T>
T* SmartPointer<T>::m_NullPtr = NULL;

#define sp SmartPointer
#endif

//EOF