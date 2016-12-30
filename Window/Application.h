#pragma once

#include	<d3d9.h>

class CApplication
{
private:
	HWND			m_hwnd;						// �E�C���h�E�n���h��
	MSG				m_msg;						// ���b�Z�[�W�\����
	WNDCLASSEX		m_wcex;						// �E�C���h�E�N���X�\����
	int				m_width;					// �E�C���h�E�̕� �v�Z�p���[�N
	int				m_height;					// �E�C���h�E�̍��� �v�Z�p���[�N
	int				m_timerid;					// �^�C�}�h�c
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

