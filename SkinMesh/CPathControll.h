//============================================================
//!	@file		CPathControll.h
//!	@brief		
//!	@note		LastEdited //
//!	@author		�n�g�������Q�[���w�ȗL�u
//============================================================

// �C���N���[�h�K�[�h
#ifndef _INC_CPATHCONTROLL_H
#define _INC_CPATHCONTROLL_H

#include <Windows.h>
#include <string>

using namespace std;

//============================================================
//!	@class	CPathControll
//!	@brief	�p�X����p�N���X�ł���������
//============================================================
class CPathControll
{
private:
	CPathControll( void ){
	};
public:
	static CPathControll& Instance( void ){
		static CPathControll inst;
		return inst;
	}

	string GetAlternatePath( LPCSTR _src ){
		int path_found = 0;
		string path = _src;
		string slash = "/";
		string newpath;
		for( unsigned int i = 0; i < path.length(); ++i ){
			if( path.at(i) == slash.at(0) )
			{
				path_found = i;
			}
		}
		if( path_found != 0 )
		{
			newpath = path.substr(0, path_found+1);
			return newpath;
		}
		else
		{
			return path;
		}
	}

};

#endif

//EOF