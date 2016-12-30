#include "SimpleCamera.h"


CSimpleCamera::CSimpleCamera()
{
}


CSimpleCamera::~CSimpleCamera()
{
}


void CSimpleCamera::SetViewport()
{
	m_port = { 0, 0, (DWORD)m_width, (DWORD)m_height, 0.0f, 1.0f };
	m_lpd3ddevice->SetViewport(&m_port);	//描画領域を変更（ビューポート行列）
}

void CSimpleCamera::SetProjection(){
	// プロジェクション変換行列作成
	D3DXMatrixPerspectiveFovLH(&m_MatProjection, D3DX_PI / 4, (float)m_width / (float)m_height, 1.0f, 100000.0f);
	m_lpd3ddevice->SetTransform(D3DTS_PROJECTION, &m_MatProjection);
}

void CSimpleCamera::SetCamera(){
	m_lpd3ddevice->SetTransform(D3DTS_VIEW, &m_MatView);	// カメラ行列をグラフィックカードにセット
}

void CSimpleCamera::Init()
{
	m_Pos = { 0, 0, 0 };
	m_Lookat = { 0, 0, 0 };
	m_Up = { 0, 1, 0 };
	SetProjection();
	GetScreenMatrix();
}

void CSimpleCamera::Update()
{
	D3DXMatrixLookAtLH(&m_MatView, &m_Pos, &m_Lookat, &m_Up);
}

void CSimpleCamera::Render()
{
	SetViewport();
	SetCamera();
}


D3DXVECTOR3 CSimpleCamera::InverseScreen(const D3DXVECTOR3& trans)
{
	D3DXMATRIX matViewToScreen;
	D3DXVECTOR3 out;
	D3DXMatrixMultiply(&matViewToScreen, &m_MatView, &m_MatProjection);
	D3DXMatrixMultiply(&matViewToScreen, &matViewToScreen, &m_MatScreen);
	D3DXMatrixInverse(&matViewToScreen, NULL, &matViewToScreen);
	D3DXVec3TransformCoord(&out, &trans, &matViewToScreen);
	return out;
}

void CSimpleCamera::GetScreenMatrix()
{
	float width = (float)m_width;
	float height = (float)m_height;
	D3DXMatrixIdentity(&m_MatScreen);
	width /= 2.0f;
	height /= 2.0f;
	m_MatScreen._11 = width;
	m_MatScreen._22 = -height;
	m_MatScreen._41 = width;
	m_MatScreen._42 = height;
}