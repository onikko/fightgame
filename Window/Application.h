#pragma once

#include	<d3d9.h>

class CApplication
{
private:
	HWND			m_hwnd;						// ウインドウハンドル
	MSG				m_msg;						// メッセージ構造体
	WNDCLASSEX		m_wcex;						// ウインドウクラス構造体
	int				m_width;					// ウインドウの幅 計算用ワーク
	int				m_height;					// ウインドウの高さ 計算用ワーク
	int				m_timerid;					// タイマＩＤ
public:
	CApplication();
	~CApplication();
	int Init(HINSTANCE, HINSTANCE, LPSTR, int);
	void Loop();
	void Destroy();
	MSG Getmsg() const{
		return m_msg;
	}
};

