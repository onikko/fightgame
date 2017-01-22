#include "UserInterface.h"



CUserInterface::CUserInterface()
{
	m_font = new CDebugFont(70);
	flag = false;
}


CUserInterface::~CUserInterface()
{
	SAFE_DELETE(m_font);
}


CTime::CTime()
{
	D3DXCreateTextureFromFile(m_lpd3ddevice, "asset/images/numfont.png", &m_pd3dTex);
	printf("Timeを読み込みました\n");
}

CTime::~CTime()
{
	SAFE_RELEASE(m_pd3dTex);
	printf("Timeを破棄しました\n");
}

void CTime::Init()
{
	camera = (CCamaraTPS*)FindItemBox("TPScamera");
	m_time = 99;
	m_cnt = 0;
}

void CTime::Update()
{
	if (!m_UpdateFlag) return;
	DWORD now_time = GetMMTime();	//今の時間を取得
	m_cnt++;
	if ( now_time >= old_time + 100000 / 60){
		m_time--;
		m_cnt = 0;
		old_time = now_time;
	}
	if (m_time < 0)	m_time = 0;
	one = (m_time / 1) % 10;
	ten = (m_time / 10) % 10;
}

void CTime::Render()
{
	float sw = (float)m_width;
	float sh = (float)m_height;
	float x = sw*0.45f;
	float y = sh*0.13f;
	float w = -sw*0.005f;
	float h = sh*0.01f;
	//タイム十の位
	vec3Trans = D3DXVECTOR3(x, y, 0.99f);
	vec3Trans = camera->InverseScreen(vec3Trans);
	BatchProcessing(vec3Trans, ten, -w, h, camera->GetViewMatrix(), m_pd3dTex);
	x = sw*0.5f;
	//タイム一の位
	vec3Trans = D3DXVECTOR3(x, y, 0.99f);
	vec3Trans = camera->InverseScreen(vec3Trans);
	BatchProcessing(vec3Trans, one, -w, h, camera->GetViewMatrix(), m_pd3dTex);
}

void CTime::BatchProcessing(D3DXVECTOR3 in, int num, float x, float y, const D3DXMATRIX& matView, LPDIRECT3DTEXTURE9 tex)
{
	BillBoard.SetPosition(in.x, in.y, in.z);
	BillBoard.InitCalcVertex(num, x, y);
	BillBoard.Draw(m_lpd3ddevice, camera->GetViewMatrix(), tex);
}

DWORD CTime::GetMMTime(void){
	timeBeginPeriod(1);	//時間の細かさを指定１だとミリ秒単位
	DWORD time = timeGetTime();
	timeEndPeriod(1);	//時間の細かさを元に戻す
	return time;
}