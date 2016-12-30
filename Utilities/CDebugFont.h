#pragma once
#include "GameObj.h"
#include "SafeDelete.h"

class CDebugFont : CGameObj
{
private:
	LPD3DXFONT		m_pFont;
public:
	CDebugFont(){
		m_pFont = NULL;
		CreateFont(10);
	}
	CDebugFont::CDebugFont(int fontsize)
	{
		m_pFont = NULL;
		CreateFont(fontsize);
	}
	~CDebugFont(){
		SAFE_RELEASE(m_pFont);
	}

	bool CreateFont(int size){
		SAFE_RELEASE(m_pFont);
		int result=D3DXCreateFont(
						m_lpd3ddevice,				// �f�o�C�X�I�u�W�F�N�g
						size,						// �����̍���
						NULL,						// �����̕�
						FW_THIN,					// �����̑���
						NULL,						// �~�b�v�}�b�v���x���̐�
						FALSE,						// �C�^���b�N�̂��ۂ� true:�C�^���b�N��
						SHIFTJIS_CHARSET,			// �����Z�b�g
						OUT_DEFAULT_PRECIS,			// ��ɂs�q�t�d�s�x�o�d�̃t�H���g���g��
						PROOF_QUALITY,				// �o�͕i��
						FIXED_PITCH | FF_MODERN,	// �s�b�`�ƃt�@�~���[�C���f�b�N�X
						"�l�r�S�V�b�N",				// �t�H���g��
						&m_pFont);					// �������ꂽ�t�H���g
		return (result == 0) ? FALSE : TRUE;
	}

	void CDebugFont::Draw(LPCTSTR text, int count, LPRECT pRect, D3DXCOLOR Color)
	{
		if (m_pFont){
			m_lpsprite->Begin(NULL);	// �Q�c�̕`��J�n��錾
			D3DXMATRIX mtrx;
			D3DXMatrixTranslation(&mtrx, 0.0f, 0.0f, 0.0f);
			m_lpsprite->SetTransform(&mtrx);

			m_pFont->DrawText(NULL, text, count, pRect, DT_CALCRECT, NULL);
			m_pFont->DrawText(NULL, text, count, pRect, DT_LEFT | DT_BOTTOM, Color);

			m_lpsprite->End();			// �`��I���̐錾
		}
	}

	void CDebugFont::Draw(LPCTSTR text, int x, int y)
	{
		// �Œ���̎w�肾���ł��ނ��̂�����
		RECT rect = { x, y, x, y };
		Draw(text, -1, &rect, 0xFFFFFFFF);
	}

	void DrawText(int x, int y, const char* str) const {
		RECT	rect = { x, y, 0, 0 };
		// ������̃T�C�Y���v�Z
		m_pFont->DrawText(NULL, str, -1, &rect, DT_CALCRECT, NULL);
		// ������`��i�ԐF�j
		m_pFont->DrawText(NULL, str, -1, &rect, DT_LEFT | DT_BOTTOM, 0xffff0000);
	}
};