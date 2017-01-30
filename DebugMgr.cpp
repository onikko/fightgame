#include "DebugMgr.h"

#define INPUT_FONT_X (5)
#define INPUT_FONT_Y (170)
#define MAX_INPUT_FONT (10)
#define MARGIN_BTM (30)
#define P1_POS_X (5)
#define P1_POS_Y (70)
#define P2_POS_X (5)
#define P2_POS_Y (100)

typedef struct{
	TCHAR data[50];
	int font_x, font_y;
}INPUT_DATA;

INPUT_DATA input_data[10];

typedef struct{
	D3DXVECTOR3 pos;
	float size_x, size_y;
	int id;
}DATA;

DATA data[10];

CDebugMgr::CDebugMgr()
{
	m_font = new CDebugFont(25);
	/*t = new CTexture(m_lpd3ddevice, "asset/images/signpost.png");*/
	D3DXCreateTextureFromFile(m_lpd3ddevice, "asset/images/signpost.png", &m_pd3dTex);
	printf("Debugを読み込みました\n");
}


CDebugMgr::~CDebugMgr()
{
	SAFE_RELEASE(m_pd3dTex);
	SAFE_DELETE(m_font);
	printf("Debugを破棄しました\n");
}

void CDebugMgr::Init()
{
	/*player = (CPlayer*)FindItemBox("player");
	enemy = (CEnemy*)FindItemBox("enemy");*/
	camera = (CCamaraTPS*)FindItemBox("TPScamera");
	for (int i = 0; i < MAX_INPUT_FONT; i++){
		sprintf((char *)input_data[i].data, "");
		input_data[i].font_x = INPUT_FONT_X;
		input_data[i].font_y = INPUT_FONT_Y + i * MARGIN_BTM;
		data[i].pos = D3DXVECTOR3(10.0f, 200.0f + i * MARGIN_BTM, 0.0f);
		data[i].size_x = 0.06f; 
		data[i].size_y = 0.06f;
		data[i].id = 9;
	}
	work_id = 9;
	time = NULL;
	frame = NULL;
	av_frame = NULL;
	fps[3600] = { NULL };//計測は1時間以内にすること
	avg = NULL;
	first2 = NULL;
	old_input = NULL;
	flag = false;
	input = NULL;
}

void CDebugMgr::Update()
{
	if (!m_UpdateFlag) return;
	//入力が前と同じなら
	if (old_input == input){
		sprintf((char *)work_font, "");
		work_id = 9;
		flag = false;
	}
	old_input = input;

	//入力を１つ移動する
	if (flag == true){
		for (int i = MAX_INPUT_FONT - 1; i > 0; i--){
			sprintf((char *)input_data[i].data, input_data[i - 1].data);
			data[i].id = data[i - 1].id;
		}
		flag = false;

		sprintf((char *)input_data[0].data, work_font);
		data[0].id = work_id;
	}
	sprintf((char *)work_font, "");
	work_id = 9;
	flag = false;

	//FPS計測
	frame++;
	if (timeGetTime() - time>1000)
	{
		first2++;
		fps[av_frame] = frame;
		if (first2>2)
		{
			av_frame++;
			avg = 0;
			for (int i = 0; i<av_frame; i++)
			{
				avg += fps[i];
			}
			avg = avg / av_frame;
		}
		sprintf(time_font, "fps=%d", frame);
		time = timeGetTime();
		frame = 0;
	}
}

void CDebugMgr::Render()
{
	D3DXVECTOR3 vec3Trans_work;
	//入力
	for (int i = 0; i < MAX_INPUT_FONT; i++){
		SetRect(&rc, input_data[i].font_x, input_data[i].font_y, 0, 0);
		m_font->Draw(input_data[i].data, -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));
		//t->Draw(0, 0, data[i].size_x, data[i].size_y, data[i].id, D3DCOLOR_ARGB(255, 255, 255, 255));
		/*vec3Trans_work = camera->InverseScreen(data[i].pos);
		BillBoard.SetPosition(vec3Trans_work.x, vec3Trans_work.y, vec3Trans_work.z);
		BillBoard.InitCalcVertex(data[i].id, data[i].size_x, data[i].size_y);
		BillBoard.Draw(m_lpd3ddevice, camera->GetViewMatrix(), m_pd3dTex);*/
	}

	if (!m_UpdateFlag){
		SetRect(&rc, 140, 140, 0, 0);
		m_font->Draw("一時停止中", -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));
	}
	//FPS計測
	SetRect(&rc, FPS_COUNT_X, FPS_COUNT_Y, 0, 0);
	m_font->Draw(time_font, -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));

	////P1位置
	//sprintf((char *)str1, "P1の現在位置：%.3f,%.3f,%.3f", player->GetMatrixWorld()._41, player->GetMatrixWorld()._42, player->GetMatrixWorld()._43);
	//SetRect(&rc, P1_POS_X, P1_POS_Y, 0, 0);
	//m_font->Draw(str1, -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));

	////P2位置
	//sprintf((char *)str1, "P2の現在位置：%.3f,%.3f,%.3f", enemy->GetMatrixWorld()._41, enemy->GetMatrixWorld()._42, enemy->GetMatrixWorld()._43);
	//SetRect(&rc, P2_POS_X, P2_POS_Y, 0, 0);
	//m_font->Draw(str1, -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));

}

void CDebugMgr::Draw(char* font, int x, int y)
{
	SetRect(&rc, x, y, 0, 0);
	m_font->Draw(font, -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));
}

void CDebugMgr::SetDeta(char* in_work, bool in_flag){
	sprintf((char *)work_font, in_work);
	flag = in_flag;
}

void CDebugMgr::SetInput(int in_input){
	input = in_input;
}

void CDebugMgr::SetId(int id){
	work_id = id;
}