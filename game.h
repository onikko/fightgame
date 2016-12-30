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
	HANDLE m_hEventHandle;		// イベントハンドル
	bool m_EndFlag = false;	// 終了フラグ
	thread m_gamemainthread;	// ゲームメインスレッド
public:
	bool GameInit(HINSTANCE hinst, HWND hwnd, int width, int height, bool fullscreen);
	void GameMain();
	void GameExit();
	void GameSetEvent();
	void GameSetEndFlag();
};