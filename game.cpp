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
	bool sts = GAME.Initialize(hwnd, hinst, fullscreen, width, height);

	if (!sts){
		MessageBox(hwnd, "ERROR!!", "DirectX �������G���[", MB_OK);
		return false;
	}
	INPUT.Init(hinst, hwnd);

	GAME.AppendObj(new CBiginRender(), 0, true);
	GAME.AppendObj(new CEndRender(), INT_MAX, true);
	GAME.AppendObj(new CTitle(), 1, true);


	if (FAILED(sts)) MessageBox(hwnd, "ERROR!!", "�ǂݍ��݃G���[", MB_OK);

	// �C�x���g�n���h������
	m_hEventHandle = CreateEvent(NULL, false, false, NULL);
	if (m_hEventHandle == NULL){
		MessageBox(hwnd, "ERROR!!", "CreateEvent �G���[", MB_OK);
		return false;
	}

	// �X���b�h����(�Q�[�����C��)
	m_gamemainthread = thread(&CGame::GameMain, this);

	return	true;
}

void CGame::GameMain()
{
	while (1){
		DWORD sts = WaitForSingleObject(m_hEventHandle, 1000);	// �C�x���g�t���O���Z�b�g�����̂�҂i1000ms�Ń^�C���A�E�g�j
		if (sts == WAIT_FAILED){
			break;
		}
		else if (sts == WAIT_TIMEOUT){
			if (m_EndFlag){
				break;
			}
			continue;
		}
		INPUT.GetKeyBuffer();
		GAME.DoInputTasks();
		GAME.DoUpdateTasks();
		GAME.DoRenderTasks();
	}
}

void CGame::GameExit()
{
	CGameObj g_DXGrobj = GAME;

	m_gamemainthread.join();					// �Q�[�����C���X���b�h�̏I����҂�

	CloseHandle(m_hEventHandle);				// �C�x���g�n���h���N���[�Y

	g_DXGrobj.Uninitialize();
}

void CGame::GameSetEvent(){
	if (!m_EndFlag){
		SetEvent(m_hEventHandle);	// �I���t���O���Z�b�g����Ă��Ȃ���΃C�x���g���Z�b�g����
	}
}

void CGame::GameSetEndFlag(){
	m_EndFlag = true;				// �I���t���O���I���ɂ���
}