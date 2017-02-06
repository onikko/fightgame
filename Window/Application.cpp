#include "Application.h"
#include "../game.h"
#include "../Scene/LoadData.h"

#define		NAME			"win32"
#define		TITLE			"game"
#define		FULLSCREEN		0				// �t���X�N���[���t���O
#define		SCREEN_X		800
#define		SCREEN_Y		600

CGame g_game;
FILE*	fp;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);			// �E�C���h�E�v���V�[�W��
void	CALLBACK TimerProc(UINT, UINT, DWORD, DWORD, DWORD);	// �^�C�}����

CApplication::CApplication()
{
	m_width = SCREEN_X;
	m_height = SCREEN_Y;
	m_timerid = 0;
}


CApplication::~CApplication()
{
}

int CApplication::Init(	HINSTANCE 	hInstance, 		// �A�v���P�[�V�����̃n���h��
						HINSTANCE 	hPrevInstance,	// ����Windows�o�[�W�����ł͎g���Ȃ�
						LPSTR 		lpszArgs, 		// �N�����̈����i������j
						int 		nWinMode)		// �E�C���h�E�\�����[�h
{

	// �R���\�[�������蓖�Ă�
	AllocConsole();

	// �W���o�͂̊��蓖��
	freopen_s(&fp, "CON", "w", stdout);

	// �������[���[�N�����o
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// �E�C���h�E�N���X���̃Z�b�g
	m_wcex.hInstance = hInstance;			// �C���X�^���X�l�̃Z�b�g
	m_wcex.lpszClassName = NAME;					// �N���X��
	m_wcex.lpfnWndProc = (WNDPROC)WndProc;		// �E�C���h�E���b�Z�[�W�֐�
	m_wcex.style = 0;					// �E�C���h�E�X�^�C��
	m_wcex.cbSize = sizeof(WNDCLASSEX);	// �\���̂̃T�C�Y
	m_wcex.hIcon = LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);	// ���[�W�A�C�R��
	m_wcex.hIconSm = LoadIcon((HINSTANCE)NULL, IDI_WINLOGO);		// �X���[���A�C�R��
	m_wcex.hCursor = LoadCursor((HINSTANCE)NULL, IDC_ARROW);		// �J�[�\���X�^�C��
	m_wcex.lpszMenuName = 0; 					// ���j���[�Ȃ�
	m_wcex.cbClsExtra = 0;					// �G�L�X�g���Ȃ�
	m_wcex.cbWndExtra = 0;
	m_wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);		// �w�i�F��

	if (!RegisterClassEx(&m_wcex)) return FALSE;	// �E�C���h�E�N���X�̓o�^

	if (FULLSCREEN){
		m_hwnd = CreateWindow(
			NAME,							// �E�B���h�E�N���X�̖��O
			TITLE,							// �^�C�g��
			WS_VISIBLE | WS_POPUP,			// �E�B���h�E�X�^�C��
			0, 0,							// �E�B���h�E�ʒu �c, ��
			SCREEN_X, SCREEN_Y,				// �E�B���h�E�T�C�Y
			NULL,							// �e�E�B���h�E�Ȃ�
			(HMENU)NULL,					// ���j���[�Ȃ�
			hInstance,						// �C���X�^���X�n���h��
			(LPVOID)NULL);					// �ǉ������Ȃ�
	}
	else{
		RECT	rWindow, rClient;
		m_hwnd = CreateWindow(
			NAME,							// �E�B���h�E�N���X�̖��O
			TITLE,							// �^�C�g��
			WS_CAPTION | WS_SYSMENU,		// �E�B���h�E�X�^�C��
			0, 0,							// �E�B���h�E�ʒu �c, ��(���ƂŒ����Ɉړ������܂�)
			SCREEN_X, SCREEN_Y,				// �E�B���h�E�T�C�Y
			HWND_DESKTOP,					// �e�E�B���h�E�Ȃ�
			(HMENU)NULL,					// ���j���[�Ȃ�
			hInstance,						// �C���X�^���X�n���h��
			(LPVOID)NULL);					// �ǉ������Ȃ�

		// �E�C���h�E�T�C�Y���Čv�Z�iMetrics�����ł́A�t���[���f�U�C���ŃN���C�A���g�̈�T�C�Y���ς���Ă��܂��̂Łj
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

	// �Q�[���̏�������
	if (!g_game.GameInit(hInstance, m_hwnd, m_width, m_height, false)){
		g_game.GameExit();
		MessageBox(m_hwnd, "ERROR!", "GameInit Error", MB_OK);
		return false;
	}

	// �E�C���h�E��\������
	ShowWindow(m_hwnd, nWinMode);
	UpdateWindow(m_hwnd);

	return 1;
}

void CApplication::Loop(){
	// �C�x���g�^�C�}�[���Z�b�g����
	timeBeginPeriod(1);			// �^�C�}�̕���\�͂��P�����ɂ���
	m_timerid = timeSetEvent(16, 1, TimerProc, 1, TIME_PERIODIC);

	while (1){	// ���b�Z�[�W����[�v
		if (!GetMessage(&m_msg, NULL, 0, 0)){	// ���b�Z�[�W���擾
			break;
		}
		else{
			TranslateMessage(&m_msg); 			// �������b�Z�[�W�ւ̃R���o�[�g�j
			DispatchMessage(&m_msg); 				// ���b�Z�[�W��WndProc�֑���
		}
	}
}

void CApplication::Destroy(){
	// �Q�[���I���t���O���Z�b�g����
	g_game.GameSetEndFlag();

	if (m_timerid) timeKillEvent(m_timerid);	// �}���`���f�B�A�^�C�}�̏I��
	timeEndPeriod(1);							// �^�C�}�̕���\�͂��Ƃɖ߂�

	// �W���o�̓N���[�Y
	fclose(fp);
	// �R���\�[���J��
	::FreeConsole();

	g_game.GameExit();
}

LRESULT WINAPI WndProc(	HWND hwnd, 		// �E�B���h�E�n���h��
						UINT message,	// ���b�Z�[�W���ʎq
						WPARAM wParam,	// �t�я��P
						LPARAM lParam)	// �t�я��Q
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
	g_game.GameSetEvent();			// �C�x���g�I�u�W�F�N�g���Z�b�g����
}