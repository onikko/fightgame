#pragma once
#include "CCameraMng.h"

#include "../Versus.h"

#define CAMERA_PRIORITY 80000
class CCamaraTPS : public CCameraMng
{
private:
	Versus* versus;
	D3DXVECTOR3 m_Pos;	//位置
	D3DXVECTOR3 m_Lookat;	// 注視点
	D3DXVECTOR3 m_Up;		// 上向き
	float m_CameraHAngle;
	float m_CameraVAngle;
	float m_SinParam;
	float m_CosParam;
	float m_Camera_Distance;
public:
	CCamaraTPS();
	~CCamaraTPS();
	void SetCamera();
	void SetViewport();
	void SetProjection();
	D3DXMATRIX GetViewMatrix(){ return m_MatView; }
	D3DXMATRIX GetProjectionMatrix(){ return m_MatProjection; }
	D3DXVECTOR3 InverseScreen(const D3DXVECTOR3& trans);
	void GetScreenMatrix();
protected:
	virtual void Init();
	virtual void Input();
	virtual void Update();
	virtual void Render();
};

