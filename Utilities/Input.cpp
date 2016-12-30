#pragma warning(disable:4267)

#include "Input.h"

#define JOYAXIS_RANGE 1000
#define JOYAXIS_MARGIN 200




//==============================================================
// CInputState�N���X

// �R���X�g���N�^
CInputState::CInputState() {
	Clear();
}

// ��Ԃ̃N���A
void CInputState::Clear() {
	Up=Down=Left=Right=false;
	Up2=Down2=Left2=Right2=false;
	for (int i=0; i<MAX_BUTTONS; i++) Button[i]=false;
	AnalogX=AnalogY=0;
	AnalogX2=AnalogY2=0;
}




//==============================================================
// �񋓗p�̃R�[���o�b�N�֐�
BOOL CALLBACK EnumJoysticksCallbackWrapper(
	const DIDEVICEINSTANCE* did, VOID* ref
) {
	CInput* input=(CInput*)ref;
	return input->EnumJoysticksCallback(did);
}

// 
BOOL CALLBACK CInput::EnumJoysticksCallback(
	const DIDEVICEINSTANCE* did
) {
	LPDIRECTINPUTDEVICE8 joystick=NULL;
	if (!FAILED(DI->CreateDevice(
		did->guidInstance, &joystick, NULL))) {
		JoySticks.push_back(joystick);
	}
	return DIENUM_CONTINUE;
}

// ���͈̔͂�ݒ肷��
BOOL CALLBACK EnumObjectsCallbackWrapper(
	const DIDEVICEOBJECTINSTANCE* dido, VOID* ref
) {
	LPDIRECTINPUTDEVICE8 joystick=(LPDIRECTINPUTDEVICE8)ref;
	if (dido->dwType & DIDFT_AXIS) {
		DIPROPRANGE dip;
		dip.diph.dwSize=sizeof(DIPROPRANGE); 
		dip.diph.dwHeaderSize=sizeof(DIPROPHEADER); 
		dip.diph.dwHow=DIPH_BYID; 
		dip.diph.dwObj=dido->dwType;
		dip.lMin=-JOYAXIS_RANGE; 
		dip.lMax=JOYAXIS_RANGE;
		joystick->SetProperty(DIPROP_RANGE, &dip.diph); 
	}	
	return DIENUM_CONTINUE;
}


CInput::~CInput() {
	FreeResources();
}


void CInput::Init(HINSTANCE hInst, HWND hwnd) {
	// DirectInput�C���^�t�F�[�X�̍쐬
	DI = NULL;
	if (FAILED(DirectInput8Create(
		hInst, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&DI, NULL))
		) return;

	// �L�[�{�[�h�f�o�C�X�̎擾�Ə�����
	Keyboard = NULL;
	if (FAILED(DI->CreateDevice(
		GUID_SysKeyboard, &Keyboard, NULL)) ||
		FAILED(Keyboard->SetDataFormat(
		&c_dfDIKeyboard)) ||
		FAILED(Keyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))
		) {
		FreeResources(); return;
	}

	// �W���C�X�e�B�b�N�f�o�C�X�̎擾�Ə�����
	if (FAILED(DI->EnumDevices(DI8DEVCLASS_GAMECTRL,
		EnumJoysticksCallbackWrapper,
		this, DIEDFL_ATTACHEDONLY))
		) {
		FreeResources(); return;
	}
	for (int i = 0, n = JoySticks.size(); i<n; i++) {
		if (FAILED(JoySticks[i]->SetDataFormat(
			&c_dfDIJoystick2)) ||
			FAILED(JoySticks[i]->SetCooperativeLevel(
			hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)) ||
			FAILED(JoySticks[i]->EnumObjects(
			EnumObjectsCallbackWrapper,
			JoySticks[i], DIDFT_ALL))
			) {
			FreeResources(); return;
		}
	}
}


//==============================================================
// �����̉��
void CInput::FreeResources() {
	if (Keyboard) {
		Keyboard->Unacquire();
		Keyboard->Release();
	}
	for (int i=0, n=JoySticks.size(); i<n; i++) {
		JoySticks[i]->Unacquire();
		JoySticks[i]->Release();
	}
	if (DI) DI->Release();
}




//==============================================================
// ���͏�Ԃ��ŐV�ɂ���
#define KEYDOWN(key) ((m_keybuffer[key]&0x80)!=0)
#define BTNDOWN(btn) ((joy_state.rgbButtons[btn]&0x80)!=0)

void CInput::UpdateState() {
	ClearState();
	// �L�[�{�[�h
	if (Keyboard){
		CInputState& s = State[0];

		if (KEYDOWN(DIK_W)){
			s.Up |= CheckKeyBuffer(DIK_W);
		}
		else if (KEYDOWN(DIK_S)){
			s.Down |= CheckKeyBuffer(DIK_S);
		}
		if (KEYDOWN(DIK_D)){
			s.Right |= CheckKeyBuffer(DIK_D);
		}
		else if (KEYDOWN(DIK_A)){
			s.Left |= CheckKeyBuffer(DIK_A);
		}
		s.Button[0] |= CheckKeyBufferTrigger(DIK_J);
		s.Button[1] |= CheckKeyBufferTrigger(DIK_K);
		s.Button[2] |= CheckKeyBufferTrigger(DIK_L);
		s.Button[3] |= CheckKeyBufferTrigger(DIK_V);
		s.Button[4] |= CheckKeyBufferTrigger(DIK_B);
		s.Button[5] |= CheckKeyBufferTrigger(DIK_N);
		s.Button[6] |= CheckKeyBufferTrigger(DIK_M);
		s.Button[7] |= CheckKeyBufferTrigger(DIK_COMMA);
		s.Button[8] |= CheckKeyBufferTrigger(DIK_PERIOD);
		s.Button[9] |= CheckKeyBufferTrigger(DIK_SLASH);
		s.Button[10] |= CheckKeyBufferTrigger(DIK_BACKSLASH);
		s.AnalogY += (s.Up ? -1 : (s.Down ? 1 : 0));
		s.AnalogX += (s.Left ? -1 : (s.Right ? 1 : 0));

		s.Up2 |= CheckKeyBuffer(DIK_E);
		s.Down2 |= CheckKeyBuffer(DIK_D);
		s.Left2 |= CheckKeyBuffer(DIK_S);
		s.Right2 |= CheckKeyBuffer(DIK_F);
		s.AnalogY2 += (s.Up2 ? -1 : (s.Down2 ? 1 : 0));
		s.AnalogX2 += (s.Left2 ? -1 : (s.Right2 ? 1 : 0));
	}

	// �W���C�X�e�B�b�N
	for (int i=0, n=JoySticks.size(); i<n; i++) {
		DIJOYSTATE2 joy_state;
		JoySticks[i]->Poll();
		if (!FAILED(JoySticks[i]->Acquire()) &&
			!FAILED(JoySticks[i]->GetDeviceState(
				sizeof(joy_state), &joy_state))
		) {
			CInputState& s=State[i];

			s.Up|=(joy_state.lY<-JOYAXIS_MARGIN);
			s.Down|=(joy_state.lY>JOYAXIS_MARGIN);
			s.Left|=(joy_state.lX<-JOYAXIS_MARGIN);
			s.Right|=(joy_state.lX>JOYAXIS_MARGIN);
			for (int j=0; j<128; j++) s.Button[j]|=BTNDOWN(j);

			// Y�����̃A�i���O����
			int jy=joy_state.lY;
			if (jy<=-JOYAXIS_RANGE+JOYAXIS_MARGIN) s.AnalogY+=-1;
			else if (-JOYAXIS_MARGIN<=jy && jy<=JOYAXIS_MARGIN) s.AnalogY+=0;
			else if (JOYAXIS_RANGE-JOYAXIS_MARGIN<=jy) s.AnalogY+=1; 
			else if (jy<0) s.AnalogY+=(float)(jy+JOYAXIS_MARGIN)/(JOYAXIS_RANGE-JOYAXIS_MARGIN*2);
			else s.AnalogY+=(float)(jy-JOYAXIS_MARGIN)/(JOYAXIS_RANGE-JOYAXIS_MARGIN*2);

			// X�����̃A�i���O����
			int jx=joy_state.lX;
			if (jx<=-JOYAXIS_RANGE+JOYAXIS_MARGIN) s.AnalogX+=-1; else 
			if (-JOYAXIS_MARGIN<=jx && jx<=JOYAXIS_MARGIN) s.AnalogX+=0; else
			if (JOYAXIS_RANGE-JOYAXIS_MARGIN<=jx) s.AnalogX+=1;
			else if (jx<0) s.AnalogX+=(float)(jx+JOYAXIS_MARGIN)/(JOYAXIS_RANGE-JOYAXIS_MARGIN*2);
			else s.AnalogX+=(float)(jx-JOYAXIS_MARGIN)/(JOYAXIS_RANGE-JOYAXIS_MARGIN*2);
		}
	}
}

void CInput::GetKeyBuffer(){
	HRESULT	hr;
	// �f�o�C�X�̔F��
	hr = Keyboard->Acquire();
	// �O��̏�Ԃ�ۑ�
	memcpy(&m_oldkeybuffer, m_keybuffer, sizeof(m_keybuffer));
	hr = Keyboard->GetDeviceState(sizeof(m_keybuffer), (LPVOID)&m_keybuffer);
	if (hr == DIERR_INPUTLOST){
		// �f�o�C�X�̔F��
		hr = Keyboard->Acquire();
	}
}

bool CInput::CheckKeyBuffer(int keyno){
	if (m_keybuffer[keyno] & 0x80){
		return true;
	}
	else{
		return false;
	}
}

bool CInput::CheckKeyBufferTrigger(int keyno){
	if (((m_keybuffer[keyno] ^ m_oldkeybuffer[keyno]) & m_keybuffer[keyno]) & 0x80){
		return true;
	}
	else{
		return false;
	}
}


//==============================================================
// �������͂���Ă��Ȃ���Ԃɂ���
void CInput::ClearState() {
	for (int i=0; i<MAX_PLAYERS; i++) {
		State[i].Clear();
	}
}


