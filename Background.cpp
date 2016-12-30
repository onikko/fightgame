#include "Background.h"
#include "Utilities\SafeDelete.h"

CBackground::CBackground()
{
	m_stage = new CDirect3DXFile(_T("asset/model/Stage.x"));
	printf("背景を読み込みました\n");
}

CBackground::CBackground(CDirect3DXFile* model)
{
	m_stage = model;
}

CBackground::~CBackground()
{
	SAFE_DELETE(m_stage);
}

void CBackground::Init()
{
	D3DXMatrixIdentity(&m_MatWorld);
}

void CBackground::Render()
{
	m_lpd3ddevice->SetTransform(D3DTS_WORLD, &m_MatWorld);	// ワールド変換行列をセット
	m_stage->Draw();
}