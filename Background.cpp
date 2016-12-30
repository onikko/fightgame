#include "Background.h"
#include "Utilities\SafeDelete.h"

CBackground::CBackground()
{
	m_stage = new CDirect3DXFile(_T("asset/model/Stage.x"));
	printf("�w�i��ǂݍ��݂܂���\n");
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
	m_lpd3ddevice->SetTransform(D3DTS_WORLD, &m_MatWorld);	// ���[���h�ϊ��s����Z�b�g
	m_stage->Draw();
}