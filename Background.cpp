#include "Background.h"
#include "Utilities\SafeDelete.h"

CBackground::CBackground()
{
	m_stage = new CDirect3DXFile(_T("asset/model/Stage.x"));
	printf("”wŒi‚ð“Ç‚Ýž‚Ý‚Ü‚µ‚½\n");
}

CBackground::CBackground(CDirect3DXFile* model)
{
	m_stage = model;
}

CBackground::~CBackground()
{
	SAFE_DELETE(m_stage);
	printf("”wŒi‚ð”jŠü‚µ‚Ü‚µ‚½\n");
}

void CBackground::Init()
{
	D3DXMatrixIdentity(&m_MatWorld);
}

void CBackground::Render()
{
	m_lpd3ddevice->SetTransform(D3DTS_WORLD, &m_MatWorld);	// ƒ[ƒ‹ƒh•ÏŠ·s—ñ‚ðƒZƒbƒg
	m_stage->Draw();
}