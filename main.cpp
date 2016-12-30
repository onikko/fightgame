//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <thread>
#include <io.h>
#include <Fcntl.h>
#include "Window/Application.h"

#if _DEBUG
#include <crtdbg.h>
#define new  new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#endif

//-----------------------------------------------------------------------------
// リンクライブラリの設定
//-----------------------------------------------------------------------------
#pragma comment(lib, "d3d9.lib")			// リンク対象ライブラリにd3d9.libを追加
#pragma comment(lib, "d3dx9.lib")			// リンク対象ライブラリにd3dx9.libを追加
#pragma comment(lib, "winmm.lib")			// リンク対象ライブラリにwinmm.libを追加
#pragma comment(lib, "dxerr.lib")

//================================================================
//	WinMain関数
//================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdshow){
	CApplication Appli;
	Appli.Init(hInstance, hPrevInstance, lpCmdLine, nCmdshow);
	Appli.Loop();
	Appli.Destroy();
	return (int)Appli.Getmsg().wParam;
}