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
// �����N���C�u�����̐ݒ�
//-----------------------------------------------------------------------------
#pragma comment(lib, "d3d9.lib")			// �����N�Ώۃ��C�u������d3d9.lib��ǉ�
#pragma comment(lib, "d3dx9.lib")			// �����N�Ώۃ��C�u������d3dx9.lib��ǉ�
#pragma comment(lib, "winmm.lib")			// �����N�Ώۃ��C�u������winmm.lib��ǉ�
#pragma comment(lib, "dxerr.lib")

//================================================================
//	WinMain�֐�
//================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdshow){
	CApplication Appli;
	Appli.Init(hInstance, hPrevInstance, lpCmdLine, nCmdshow);
	Appli.Loop();
	Appli.Destroy();
	return (int)Appli.Getmsg().wParam;
}