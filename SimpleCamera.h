#pragma once
#include "Camera\CCameraMng.h"

class CSimpleCamera : public CCameraMng
{
private:
	D3DXVECTOR3 m_Pos;	//位置
	D3DXVECTOR3 m_Lookat;	// 注視点
	D3DXVECTOR3 m_Up;		// 上向き
public:
	CSimpleCamera();
	~CSimpleCamera();
	void SetCamera();
	void SetViewport();
	void SetProjection();
	D3DXMATRIX GetViewMatrix(){ return m_MatView; }
	D3DXMATRIX GetProjectionMatrix(){ return m_MatProjection; }
	D3DXVECTOR3 InverseScreen(const D3DXVECTOR3& trans);
	void GetScreenMatrix();
protected:
	virtual void Init();
	virtual void Update();
	virtual void Render();
};

