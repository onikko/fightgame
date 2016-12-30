#pragma once
#include "../Utilities/GameObj.h"
class CCameraMng : public CGameObj
{
protected:
	D3DXMATRIX	m_MatView;			//カメラ行列
	D3DXMATRIX	m_MatProjection;	// プロジェクション変換行列
	D3DXMATRIX	m_MatScreen;
	D3DVIEWPORT9 m_port;
public:
	CCameraMng();
	~CCameraMng();
	virtual void SetCamera(){}
	virtual void SetProjection(){}
	virtual void SetViewport(){}
	virtual D3DXMATRIX GetViewMatrix() = 0;
	D3DXMATRIX GetProjectionMatrix() const{ return m_MatProjection; }
	virtual D3DXVECTOR3 InverseScreen(const D3DXVECTOR3& trans) = 0;
};

