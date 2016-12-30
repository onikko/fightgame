#define	_MAIN
#include "game.h"
#include "Utilities/SafeDelete.h"
#include "Utilities/GameObj.h"
#include "Utilities/CDebugFont.h"
#include "Utilities/Input.h"
#include "Render.h"
#include "Scene/SceneMgr.h"
#include "SkinMesh/SkinMesh.h"
#include "Utilities/Input.h"
#include "DebugMgr.h"
#include "SimpleCamera.h"

bool CGame::GameInit(HINSTANCE hinst, HWND hwnd, int width, int height, bool fullscreen)
{
	bool sts = CGameObj::Instance().Initialize(hwnd, hinst, fullscreen, width, height);

	if (!sts){
		MessageBox(hwnd, "ERROR!!", "DirectX 初期化エラー", MB_OK);
		return false;
	}
	CInput::Instance().Init(hinst, hwnd);

	CGameObj::Instance().AppendObj(new CBiginRender(), 0, true);
	CGameObj::Instance().AppendObj(new CEndRender(), INT_MAX, true);
	CGameObj::Instance().AppendObj(new CTitle(), 1, true);


	if (FAILED(sts)) MessageBox(hwnd, "ERROR!!", "読み込みエラー", MB_OK);

	// イベントハンドル生成
	m_hEventHandle = CreateEvent(NULL, false, false, NULL);
	if (m_hEventHandle == NULL){
		MessageBox(hwnd, "ERROR!!", "CreateEvent エラー", MB_OK);
		return false;
	}

	// スレッド生成(ゲームメイン)
	m_gamemainthread = thread(&CGame::GameMain, this);

	return	true;
}

void CGame::GameMain()
{
	while (1){
		DWORD sts = WaitForSingleObject(m_hEventHandle, 1000);	// イベントフラグがセットされるのを待つ（1000msでタイムアウト）
		if (sts == WAIT_FAILED){
			break;
		}
		else if (sts == WAIT_TIMEOUT){
			if (m_EndFlag){
				break;
			}
			continue;
		}
		CInput::Instance().GetKeyBuffer();
		CGameObj::Instance().DoInputTasks();
		CGameObj::Instance().DoUpdateTasks();
		CGameObj::Instance().DoRenderTasks();
	}
}

void CGame::GameExit()
{
	CGameObj g_DXGrobj = CGameObj::Instance();

	m_gamemainthread.join();					// ゲームメインスレッドの終了を待つ

	CloseHandle(m_hEventHandle);				// イベントハンドルクローズ

	g_DXGrobj.Uninitialize();
}

void CGame::GameSetEvent(){
	if (!m_EndFlag){
		SetEvent(m_hEventHandle);	// 終了フラグがセットされていなければイベントをセットする
	}
}

void CGame::GameSetEndFlag(){
	m_EndFlag = true;				// 終了フラグをオンにする
}