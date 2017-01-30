#include "SceneMgr.h"
#include "../DebugMgr.h"
#include "../Chr/CPlayer.h"
#include "../Chr/Enemy.h"
#include "../Background.h"
#include "../UI/UserInterface.h"
#include "../Camera/CamaraTPS.h"
#include "../Utilities/CDebugFont.h"
#include "../Utilities/Input.h"
#include "../HitCheck.h"
#include "../SimpleCamera.h"
#include "../Adjust.h"

#include "../Versus.h"

typedef enum{
	eMenu_Game,
	eMenu_Config,

	eMenu_Num
}eMenu;

static int Now_Select = eMenu_Game;

CSceneMgr::CSceneMgr()
{
}


CSceneMgr::~CSceneMgr()
{
}

CTitle::CTitle()
{
	AppendObj(new CLoadTitle(), 2, true);
	D3DXCreateTextureFromFile(m_lpd3ddevice, "asset/images/title.png", &m_pd3dTex);
}

CTitle::~CTitle()
{
	SAFE_RELEASE(m_pd3dTex);
}

void CTitle::Init()
{
	m_font.CreateFont(25);
	camera = (CSimpleCamera*)FindItemBox("Camera");
	billboard.SetPosition(0, 0, 0);
	billboard.SetColor(D3DCOLOR_ARGB(255, 255, 0, 0));
	billboard.SetSize(0, 0, 1.5, 1.5);
	D3DXMatrixIdentity(&a);
}

void CTitle::Input()
{
	if (CInput::Instance().CheckKeyBufferTrigger(DIK_W)){
		Now_Select = (Now_Select + 1) % eMenu_Num;
	}
	if (CInput::Instance().CheckKeyBufferTrigger(DIK_S)){
		Now_Select = (Now_Select + (eMenu_Num - 1)) % eMenu_Num;
	}
	if (CInput::Instance().CheckKeyBufferTrigger(DIK_SPACE)){
		switch (Now_Select)
		{
		case eMenu_Game:
			RemoveObj(1, INT_MAX - 1);
			AppendObj(new CLoadStage(), 1, true);
			break;
		case eMenu_Config:
			RemoveObj(1, INT_MAX - 1);
			AppendObj(new CConfig(false), 1, true);
			break;
		}
	}
}

void CTitle::Render()
{
	float sw = (float)m_width;
	float sh = (float)m_height;
	float x = sw*0.45f;
	float GAME_FONT = sh*0.5f;
	float CONFIG_FONT = sh*0.55f;
	RECT rc;
	title();
	SetRect(&rc, (int)x, (int)GAME_FONT, 0, 0);
	m_font.Draw("ゲーム", -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));
	SetRect(&rc, (int)x, (int)CONFIG_FONT, 0, 0);
	m_font.Draw("設定", -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));
	float y = 0;
	switch (Now_Select)
	{
	case eMenu_Game:
		y = GAME_FONT;
		break;
	case eMenu_Config:
		y = CONFIG_FONT;
		break;
	}
	SetRect(&rc, (int)x - 50, (int)y, 0, 0);
	m_font.Draw("→", -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));
}

void CTitle::title(){
	m_Vertex[0].x = -1;
	m_Vertex[0].y = -1;
	m_Vertex[0].z = 0.0f;
	m_Vertex[0].color = D3DCOLOR_ARGB(255, 255, 255, 0);
	m_Vertex[0].tu = 0.0f;
	m_Vertex[0].tv = 1.0f;

	m_Vertex[1].x = -1;
	m_Vertex[1].y = 1;
	m_Vertex[1].z = 0.0f;
	m_Vertex[1].color = D3DCOLOR_ARGB(255, 255, 255, 0);
	m_Vertex[1].tu = 0.0f;
	m_Vertex[1].tv = 0.0f;

	m_Vertex[2].x = 1;
	m_Vertex[2].y = 1;
	m_Vertex[2].z = 0.0f;
	m_Vertex[2].color = D3DCOLOR_ARGB(255, 255, 255, 0);
	m_Vertex[2].tu = 1.0f;
	m_Vertex[2].tv = 0.0f;

	m_Vertex[3].x = 1;
	m_Vertex[3].y = -1;
	m_Vertex[3].z = 0;
	m_Vertex[3].color = D3DCOLOR_ARGB(255, 255, 255, 0);
	m_Vertex[3].tu = 1.0f;
	m_Vertex[3].tv = 1.0f;

	//m_lpd3ddevice->SetRenderState(D3DRS_LIGHTING, false);
	//m_lpd3ddevice->SetFVF(D3DFVFCUSTOM_VERTEX);
	//m_lpd3ddevice->SetTexture(0, m_pd3dTex);
	//// ワールド変換行列をセット
	//m_lpd3ddevice->SetTransform(D3DTS_WORLD, &a);
	//m_lpd3ddevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Vertex, sizeof(MyVertex));
	//m_lpd3ddevice->SetRenderState(D3DRS_LIGHTING, true);

	D3DXVECTOR3 vec3Trans = D3DXVECTOR3(100, 100, 0.99f);
	vec3Trans = camera->InverseScreen(vec3Trans);
	BatchProcessing(vec3Trans, 200, 200, camera->GetViewMatrix(), m_pd3dTex);
}

void CTitle::BatchProcessing(D3DXVECTOR3 in, float x, float y, const D3DXMATRIX& matView, LPDIRECT3DTEXTURE9 tex)
{
	billboard.SetPosition(in.x, in.y, in.z);
	billboard.CalcVertex();
	billboard.Draw(m_lpd3ddevice, camera->GetViewMatrix(), tex);
}

// 引数gameclearがtrueならゲームクリアモード
// falseならゲームオーバーモード
CConfig::CConfig(bool gameclear)
{
	clear = gameclear;
}

void CConfig::Init()
{
	// 敵の出現をやめる
	RemoveObj(FindObj("epattern"));
}

void CConfig::Input()
{

}

void CConfig::Render()
{
	// メッセージの表示
	const TCHAR m1[] = _T("GAME CLEAR");
	const TCHAR m2[] = _T("GAME OVER");

	//if (CDirectInput::Instance().CheckKeyBufferTrigger(DIK_RETURN)){
	//	if (clear == true){
	//		// タイトルに戻る
	//		AppendObj(new CTitle(), 1, true);
	//	}
	//	else{
	//		// ステージデータを読み直す
	//		AppendObj(new CLoadStage(), 1, true);
	//	}
	//	RemoveObj(10, INT_MAX - 1);
	//	return;
	//}
}

void CLoadStage::Init()
{
	// 別スレッドでロードを開始
	//_beginthread(this->LoadThread, 0, NULL);

	m_loadthread = thread(&CLoadStage::LoadThread, this);
	RemoveObj(this);
}

std::mutex mtx1, mtx2;
void CLoadStage::LoadThread(void *data)
{
	// ここで時間のかかる処理を実行
	ListData ld;

	ResetItemBox();

	// 背景
	ld.gameobj = new CBackground();
	ld.autodelete = true;
	strcpy_s(ld.name, 32, "bg");
	ld.priority = BACKGROUND_PRIORITY;
	temp.push_back(ld);
	AppendItemBox("bg", ld.gameobj);

	//// プレイヤー
	//ld.gameobj = new CPlayer(D3DXVECTOR3(0, -90, 0), D3DXVECTOR3(-1.8f, 0, 0.), 0);
	//ld.autodelete = true;
	//strcpy_s(ld.name, 32, "player");
	//ld.priority = PLAYER_PRIORITY;
	//temp.push_back(ld);
	//AppendItemBox("player", ld.gameobj);


	//// 敵
	//ld.gameobj = new CEnemy(D3DXVECTOR3(0, 90, 0), D3DXVECTOR3(1.8f, 0, 0.), 0);
	//ld.autodelete = true;
	//strcpy_s(ld.name, 32, "enemy");
	//ld.priority = ENEMY_PRIORITY;
	//temp.push_back(ld);
	//AppendItemBox("enemy", ld.gameobj);

	/*m_loadplayer1 = thread(&CLoadStage::LoadThreadPlayer1, data);
	m_loadplayer2 = thread(&CLoadStage::LoadThreadPlayer2, data);

	m_loadplayer1.join();
	m_loadplayer2.join();*/

	//カメラ
	ld.gameobj = new CCamaraTPS();
	ld.autodelete = true;
	strcpy_s(ld.name, 32, "TPScamera");
	ld.priority = CAMERA_PRIORITY;
	temp.push_back(ld);
	AppendItemBox("TPScamera", ld.gameobj);

	ld.gameobj = new Versus();
	ld.autodelete = true;
	strcpy_s(ld.name, 32, "versus");
	ld.priority = VERSUS_PRIORITY;
	temp.push_back(ld);
	AppendItemBox("versus", ld.gameobj);

	/*ld.gameobj = new CHitCheck();
	ld.autodelete = true;
	strcpy_s(ld.name, 32, "hitcheck");
	ld.priority = HITCHECK_PRIORITY;
	temp.push_back(ld);
	AppendItemBox("hitcheck", ld.gameobj);*/

	/*ld.gameobj = new CAdjust();
	ld.autodelete = true;
	strcpy_s(ld.name, 32, "adjust");
	ld.priority = ADJUST_PRIORITY;
	temp.push_back(ld);
	AppendItemBox("adjust", ld.gameobj);*/


	ld.gameobj = new CTime();
	ld.autodelete = true;
	strcpy_s(ld.name, 32, "Time");
	ld.priority = TIME_PRIORITY;
	temp.push_back(ld);
	AppendItemBox("Time", ld.gameobj);

	ld.gameobj = new CDebugMgr();
	ld.autodelete = true;
	strcpy_s(ld.name, 32, "Debug");
	ld.priority = DEBUG_PRIORITY;
	temp.push_back(ld);
	AppendItemBox("Debug", ld.gameobj);

	/*thread_guard tg_player2(m_loadplayer2);
	thread_guard tg_player1(m_loadplayer1);*/
	complete = true;
}

void CLoadTitle::Init()
{
	m_loadthread = thread(&CLoadTitle::LoadThread, this);
	RemoveObj(this);
}

void CLoadTitle::LoadThread(void *data)
{
	// ここで時間のかかる処理を実行
	ListData ld;

	ResetItemBox();

	//カメラ
	ld.gameobj = new CSimpleCamera();
	ld.autodelete = true;
	strcpy_s(ld.name, 32, "Camera");
	ld.priority = 3;
	temp.push_back(ld);
	AppendItemBox("Camera", ld.gameobj);
	printf("カメラを読み込みました\n");

	complete = true;
}