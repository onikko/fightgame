#pragma once
#include "Camera\CCameraMng.h"

class CSimpleCamera : public CCameraMng
{
private:
	D3DXVECTOR3 m_Pos;	//à íu
	D3DXVECTOR3 m_Lookat;	// íçéãì_
	D3DXVECTOR3 m_Up;		// è„å¸Ç´
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

