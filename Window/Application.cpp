#include "Application.h"
#include "../game.h"
#include "../Scene/LoadData.h"

#define		NAME			"win32"
#define		TITLE			"game"
#define		FULLSCREEN		0				// フルスクリーンフラグ
#define		SCREEN_X		800
#define		SCREEN_Y		600

CGame g_game;
FILE*	fp;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);			// ウインドウプロシージャ
void	CALLBACK TimerProc(UINT, UINT, DWORD, DWORD, DWORD);	// タイマ処理

CApplication::CApplication()
{
	m_width = SCREEN_X;
	m_height = SCREEN_Y;
	m_timerid = 0;
}


CApplication::~CApplication()
{
}

int CApplication::Init(	HINSTANCE 	hInstance, 		// アプリケーションのハンドル
						HINSTANCE 	hPrevInstance,	// 今のWindowsバージョンでは使われない
						LPSTR 		lpszArgs, 		// 起動時の引数（文字列）
						int 		nWinMode)		// ウインドウ表示モード
{

	// コンソールを割り当てる
	AllocConsole();

	// 標準出力の割り当て
	freopen_s(&fp, "CON", "w", stdout);

	// メモリーリークを検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// ウインドウクラス情報のセット
	m_wcex.hInstance = hInstance;			// インスタンス値のセット
	m_wcex.lpszClassName = NAME;					// クラス名
	m_wcex.lpfnWndProc = (WNDPROC)WndProc;		// ウインドウメッセージ関数
	m_wcex.style = 0;					// ウインドウスタイル
	m_wcex.cbSize = sizeof(WNDCLASSEX);	// 構造体のサイズ
	m_wcex.hIcon = LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);	// ラージアイコン
	m_wcex.hIconSm = LoadIcon((HINSTANCE)NULL, IDI_WINLOGO);		// スモールアイコン
	m_wcex.hCursor = LoadCursor((HINSTANCE)NULL, IDC_ARROW);		// カーソルスタイル
	m_wcex.lpszMenuName = 0; 					// メニューなし
	m_wcex.cbClsExtra = 0;					// エキストラなし
	m_wcex.cbWndExtra = 0;
	m_wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);		// 背景色白

	if (!RegisterClassEx(&m_wcex)) return FALSE;	// ウインドウクラスの登録

	if (FULLSCREEN){
		m_hwnd = CreateWindow(
			NAME,							// ウィンドウクラスの名前
			TITLE,							// タイトル
			WS_VISIBLE | WS_POPUP,			// ウィンドウスタイル
			0, 0,							// ウィンドウ位置 縦, 横
			SCREEN_X, SCREEN_Y,				// ウィンドウサイズ
			NULL,							// 親ウィンドウなし
			(HMENU)NULL,					// メニューなし
			hInstance,						// インスタンスハンドル
			(LPVOID)NULL);					// 追加引数なし
	}
	else{
		RECT	rWindow, rClient;
		m_hwnd = CreateWindow(
			NAME,							// ウィンドウクラスの名前
			TITLE,							// タイトル
			WS_CAPTION | WS_SYSMENU,		// ウィンドウスタイル
			0, 0,							// ウィンドウ位置 縦, 横(あとで中央に移動させます)
			SCREEN_X, SCREEN_Y,				// ウィンドウサイズ
			HWND_DESKTOP,					// 親ウィンドウなし
			(HMENU)NULL,					// メニューなし
			hInstance,						// インスタンスハンドル
			(LPVOID)NULL);					// 追加引数なし

		// ウインドウサイズを再計算（Metricsだけでは、フレームデザインでクライアント領域サイズが変わってしまうので）
		GetWindowRect(m_hwnd, &rWindow);
		GetClientRect(m_hwnd, &rClient);
		m_width = (rWindow.right - rWindow.left) - (rClient.right - rClient.left) + SCREEN_X;
		m_height = (rWindow.bottom - rWindow.top) - (rClient.bottom - rClient.top) + SCREEN_Y;
		SetWindowPos(
			m_hwnd,
			NULL,
			GetSystemMetrics(SM_CXSCREEN) / 2 - m_width / 2,
			GetSystemMetrics(SM_CYSCREEN) / 2 - m_height / 2,
			m_width - 1,
			m_height - 1,
			SWP_NOZORDER);
	}

	if (!m_hwnd) return FALSE;

	// ゲームの初期処理
	if (!g_game.GameInit(hInstance, m_hwnd, m_width, m_height, false)){
		g_game.GameExit();
		MessageBox(m_hwnd, "ERROR!", "GameInit Error", MB_OK);
		return false;
	}

	// ウインドウを表示する
	ShowWindow(m_hwnd, nWinMode);
	UpdateWindow(m_hwnd);

	return 1;
}

void CApplication::Loop(){
	// イベントタイマーをセットする
	timeBeginPeriod(1);			// タイマの分解能力を１ｍｓにする
	m_timerid = timeSetEvent(16, 1, TimerProc, 1, TIME_PERIODIC);

	while (1){	// メッセージ･ループ
		if (!GetMessage(&m_msg, NULL, 0, 0)){	// メッセージを取得
			break;
		}
		else{
			TranslateMessage(&m_msg); 			// 文字メッセージへのコンバート）
			DispatchMessage(&m_msg); 				// メッセージをWndProcへ送る
		}
	}
}

void CApplication::Destroy(){
	// ゲーム終了フラグをセットする
	g_game.GameSetEndFlag();

	if (m_timerid) timeKillEvent(m_timerid);	// マルチメディアタイマの終了
	timeEndPeriod(1);							// タイマの分解能力もとに戻す

	// 標準出力クローズ
	fclose(fp);
	// コンソール開放
	::FreeConsole();

	g_game.GameExit();
}

LRESULT WINAPI WndProc(	HWND hwnd, 		// ウィンドウハンドル
						UINT message,	// メッセージ識別子
						WPARAM wParam,	// 付帯情報１
						LPARAM lParam)	// 付帯情報２
{
	switch (message){
	case WM_KEYDOWN:
		switch (wParam){
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

void CALLBACK TimerProc(UINT, UINT, DWORD, DWORD, DWORD)
{
	g_game.GameSetEvent();			// イベントオブジェクトをセットする
}