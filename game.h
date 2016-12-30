#pragma once
#include <Windows.h>
#include <thread>
#include <list>
#include <string.h>
#include <memory>
#include <algorithm>

using namespace std;
class CGame
{
private:
	HANDLE m_hEventHandle;		// �C�x���g�n���h��
	bool m_EndFlag = false;	// �I���t���O
	thread m_gamemainthread;	// �Q�[�����C���X���b�h
public:
	bool GameInit(HINSTANCE hinst, HWND hwnd, int width, int height, bool fullscreen);
	void GameMain();
	void GameExit();
	void GameSetEvent();
	void GameSetEndFlag();
};