//#ifndef SAFE_DELETE
//#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
//#endif    
//#ifndef SAFE_DELETE_ARRAY
//#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
//#endif    
//#ifndef SAFE_RELEASE
//#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
//#endif

#ifndef __COMMON__
#define __COMMON__
#include <windows.h>
//=================================================================================================
//Å@COMMON
//=================================================================================================
template <typename T>
inline void SafeRelease(T& ptr)
{
	if (ptr)
	{
		ptr->Release();
		ptr = nullptr;
	}
}
#define SAFE_RELEASE SafeRelease 

template <typename T>
inline void SafeDelete(T& ptr)
{
	if (ptr)
	{
		delete ptr;
		ptr = nullptr;
	}
}
#define SAFE_DELETE SafeDelete  

template <typename T>
inline void SafeDeleteArray(T& ptr)
{
	if (ptr)
	{
		delete[] ptr;
		ptr = nullptr;
	}
}
#define SAFE_DELETE_ARRAY SafeDeleteArray

inline	void	SafeClose(HANDLE h)
{
	if (h != NULL)
	{
		CloseHandle(h);
		h = NULL;
	}
}
#define	SAFE_CLOSE_HANDLE SafeClose

#endif