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
						m_lpd3ddevice,				// デバイスオブジェクト
						size,						// 文字の高さ
						NULL,						// 文字の幅
						FW_THIN,					// 文字の太さ
						NULL,						// ミップマップレベルの数
						FALSE,						// イタリック体か否か true:イタリック体
						SHIFTJIS_CHARSET,			// 文字セット
						OUT_DEFAULT_PRECIS,			// 常にＴＲＵＥＴＹＰＥのフォントを使う
						PROOF_QUALITY,				// 出力品質
						FIXED_PITCH | FF_MODERN,	// ピッチとファミリーインデックス
						"ＭＳゴシック",				// フォント名
						&m_pFont);					// 生成されたフォント
		return (result == 0) ? FALSE : TRUE;
	}

	void CDebugFont::Draw(LPCTSTR text, int count, LPRECT pRect, D3DXCOLOR Color)
	{
		if (m_pFont){
			m_lpsprite->Begin(NULL);	// ２Ｄの描画開始を宣言
			D3DXMATRIX mtrx;
			D3DXMatrixTranslation(&mtrx, 0.0f, 0.0f, 0.0f);
			m_lpsprite->SetTransform(&mtrx);

			m_pFont->DrawText(NULL, text, count, pRect, DT_CALCRECT, NULL);
			m_pFont->DrawText(NULL, text, count, pRect, DT_LEFT | DT_BOTTOM, Color);

			m_lpsprite->End();			// 描画終了の宣言
		}
	}

	void CDebugFont::Draw(LPCTSTR text, int x, int y)
	{
		// 最低限の指定だけですむものぐさ版
		RECT rect = { x, y, x, y };
		Draw(text, -1, &rect, 0xFFFFFFFF);
	}

	void DrawText(int x, int y, const char* str) const {
		RECT	rect = { x, y, 0, 0 };
		// 文字列のサイズを計算
		m_pFont->DrawText(NULL, str, -1, &rect, DT_CALCRECT, NULL);
		// 文字列描画（赤色）
		m_pFont->DrawText(NULL, str, -1, &rect, DT_LEFT | DT_BOTTOM, 0xffff0000);
	}
};