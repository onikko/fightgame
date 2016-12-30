#pragma once
#include "GameObj.h"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <vector>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#define MAX_PLAYERS 16
#define MAX_BUTTONS 128


// É}ÉEÉXèÓïÒ
typedef struct {
	int X, Y;
	bool LButton, MButton, RButton;
} MOUSE_STATE;


class CInputState {
private:
public:
	bool Up, Down, Left, Right, Button[MAX_BUTTONS],
		Up2, Down2, Left2, Right2;
	float AnalogX, AnalogY, AnalogX2, AnalogY2;
	CInputState();
	void Clear();
};




class CInput{
private:
	CInputState State[MAX_PLAYERS];
	LPDIRECTINPUT8 DI;
	LPDIRECTINPUTDEVICE8 Keyboard;
	vector<LPDIRECTINPUTDEVICE8> JoySticks;
	char	m_keybuffer[256];
	char	m_oldkeybuffer[256];
	CInput(){
		DI = NULL;
		Keyboard = NULL;
	}
	void operator=(const CInput& obj) {}
	CInput(const CInput &obj) {}
public:
	~CInput();
	static CInput &Instance(void)
	{
		static CInput instance;
		return instance;
	}
	void Init(HINSTANCE hInst, HWND hwnd);
	void UpdateState();
	void FreeResources();
	const CInputState* GetState(int player) {
		return &State[player];
	}
	void ClearState();
	void GetKeyBuffer();
	bool CheckKeyBuffer(int keyno);
	bool CheckKeyBufferTrigger(int keyno);
	BOOL CALLBACK EnumJoysticksCallback(
		const DIDEVICEINSTANCE* did);
};
