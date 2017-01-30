#include "Enemy.h"
#include "../Utilities/SafeDelete.h"
#include "../HitBox.h"
#include "../Input_macro.h"
#include "../LoadHitData.h"

#define PLAYER_SPEED (0.02f)

CHitBox* HitBall;
SmartPointer<CSkinMeshEffect> g_effect(new CSkinMeshEffect());

string hit_attack_name[HIT_ATTACK_POS];
string hit_defense_name[HIT_DEFENSE_POS];

typedef struct {
	LPCTSTR g_vmdname;
	bool loop_flag;
}VMDDATA;
VMDDATA vmddata[] = {
	{ "asset/motion/down.vmd", false },
	{ "asset/motion/stand_1.vmd", true },
	{ "asset/motion/walk_1.vmd", true },
	{ "asset/motion/垂直ジャンプ.vmd", true },
	{ "asset/motion/pan.vmd", false },
	{ "asset/motion/kik.vmd", false }
};

typedef enum{
	DOWN,
	STAND,
	WALK,
	JUMP,
	SMALL_PUNCH,
	SMALL_KIKKU,
	SAMPLE,
	ANIM_MAX
}ANIM;

struct MOTION {
	int AnimIndex;
	const int* Command;
};

MOTION g_Motion[] = {
	{ SMALL_KIKKU, CMD5K },
	{ SMALL_PUNCH, CMD5P },
	{ DOWN, CMD2 },
	{ WALK, CMD4 },
	{ WALK, CMD6 },
	{ STAND, CMD5 },
};

//--------------------------------------------------------------
// CEnemy
//--------------------------------------------------------------
CEnemy::CEnemy(D3DXVECTOR3 angle, D3DXVECTOR3 translation, bool moveflag)
{
	m_Angle = { angle.x, angle.y, angle.z };
	m_Translation = { translation.x, translation.y, translation.z };
	m_jumpflag = 0;
	m_motionflag = m_old_motionflag = STAND;
	m_rigidity = 0;
	m_moveflag = moveflag;
	m_Speed = 0.0f;
	Reverse = false;
	for (int i = 0; i<INPUT_COUNT; i++) m_nInput[i] = INPUT_NULL;
	bool sts;
	//CGeneralFactory<PmxSkinMesh>::Instance().Create(1, 10, m_model);
	m_model = new PmxSkinMesh();
	//m_model->SetDevice(m_lpd3ddevice);
	//sts = m_model->LoadModel("asset/model/sakuya/sakuya.pmx", m_lpd3ddevice);
	//sts = m_model->LoadModel("model/駆逐艦天津風1.1/天津風（艤装なし）.pmx", m_lpd3ddevice);
	if (!sts){
		MessageBox(NULL, "ERROR!!", "load model", MB_OK);
		return;
	}

	//データロード
	LoadData();

	////当たり判定（防御）
	//for (int i = 0; i < HIT_DEFENSE_POS; i++){
	//	g_HitDefense[i].id = m_model->GetBoneId(hit_defense_name[i]);
	//	g_HitDefense[i].x = m_model->GetBoneAddress()->operator[](g_HitDefense[i].id).GetModelLocalPosition().x;
	//	g_HitDefense[i].y = m_model->GetBoneAddress()->operator[](g_HitDefense[i].id).GetModelLocalPosition().y;
	//	g_HitDefense[i].z = m_model->GetBoneAddress()->operator[](g_HitDefense[i].id).GetModelLocalPosition().z;
	//	D3DXMatrixIdentity(&g_HitDefense[i].world);
	//	g_HitDefense[i].flag = true;
	//}
	////当たり判定（攻撃）
	//for (int i = 0; i < HIT_ATTACK_POS; i++){
	//	g_HitAttack[i].id = m_model->GetBoneId(hit_attack_name[i]);
	//	g_HitAttack[i].x = m_model->GetBoneAddress()->operator[](g_HitAttack[i].id).GetModelLocalPosition().x;
	//	g_HitAttack[i].y = m_model->GetBoneAddress()->operator[](g_HitAttack[i].id).GetModelLocalPosition().y;
	//	g_HitAttack[i].z = m_model->GetBoneAddress()->operator[](g_HitAttack[i].id).GetModelLocalPosition().z;
	//	D3DXMatrixIdentity(&g_HitAttack[i].world);
	//	g_HitAttack[i].flag = true;
	//}

	for (int i = 0; i < ANIM_MAX; i++){
		//CGeneralFactory<VmdMotionController>::Instance().Create(1, i, m_motion[i]);
		m_motion[i] = new VmdMotionController();
		m_motion[i]->LoadVmdFile(vmddata[i].g_vmdname, m_model->GetBoneAddress(), m_model->GetIkAddress(), vmddata[i].loop_flag);
	}
	g_effect->LoadEffect("effect/skinmesh.fx");
	gauge = new CGauge(1);
	HitBall = new CHitBox(_T("asset/model/Ball.x"));
	printf("P2を読み込みました\n");
}


CEnemy::~CEnemy()
{
	SAFE_DELETE(m_model);
	for (int i = 0; i < ANIM_MAX; i++){
		SAFE_DELETE(m_motion[i]);
	}
	SAFE_DELETE(HitBall);
	SAFE_DELETE(gauge);
	printf("P2を破棄しました\n");
}

void CEnemy::Init()
{
	//camera = (CCamaraTPS*)FindItemBox("TPScamera");
	//gauge = (CGauge*)FindItemBox("Gauge2");
	//m_debug = (CDebugMgr*)FindItemBox("Debug");
	D3DXMatrixIdentity(&m_MatTotal);
	D3DXMatrixIdentity(&m_MatWork);
	m_Speed = PLAYER_SPEED;
	m_model->SetMotion(m_motion[STAND]);
	m_model->SetTech(g_effect);
	//camera->GetViewMatrix();
	MakeTotalMatrix();
}

// 入力の判定
bool CEnemy::CheckInput(const int *command)
{
	int i = 0, j = 0;
	while (!(command[i] & INPUT_END)){
		i++;
	}
	int time = command[i] & ~INPUT_END;
	for (i--; i >= 0; i--) {
		int input = command[i];
		while (j < INPUT_COUNT && (m_nInput[j] & input) != input){
			j++;
		}
		if (j >= time || j == INPUT_COUNT){
			return false;
		}
	}
	return true;
}


void CEnemy::Input()
{

	//InputJudge();

	//if (m_rigidity == 0){
	//	CommandJudge();
	//}
	////入力がなければ
	//m_moveflag = CheckInput(CMD5) && m_rigidity == 0 ? false : true;

	//デバッグ
	if (CInput::Instance().CheckKeyBufferTrigger(DIK_F4)){
		LoadData();
	}
	/*if (CInput::Instance().CheckKeyBufferTrigger(DIK_F5)){
		for (int i = 0; i < HIT_DEFENSE_POS; i++){
			g_HitDefense[i].flag = false;
		}
		for (int i = 0; i < HIT_ATTACK_POS; i++){
			g_HitAttack[i].flag = false;
		}
	}
	if (CInput::Instance().CheckKeyBufferTrigger(DIK_F6)){
		for (int i = 0; i < HIT_DEFENSE_POS; i++){
			g_HitDefense[i].flag = true;
		}
		for (int i = 0; i < HIT_ATTACK_POS; i++){
			g_HitAttack[i].flag = true;
		}
	}*/
}

void CEnemy::Update()
{
	//if (!m_UpdateFlag) return;
	if (m_rigidity != 0){
		m_Speed = 0.0f;
	}
	if (m_moveflag){
		m_Translation.x += m_Speed;
		if (m_jumpflag == 1){
			m_Translation.y += 0.1f;
			m_motionflag = JUMP;
			if (m_MatTotal._42 > 1.0f){
				m_Translation.y = 1.0f;
				m_jumpflag = 2;
			}
		}
		if (m_jumpflag == 2){
			m_Translation.y -= 0.1f;
			m_motionflag = JUMP;
			if (m_MatTotal._42 < 0.0f){
				m_Translation.y = 0.0f;
				m_jumpflag = 0;
			}
		}
		MakeTotalMatrix();
		m_rigidity--;
	}
	else{
		m_Speed = 0;
		m_motionflag = STAND;
	}

	if (m_rigidity < 0){
		m_rigidity = 0;
	}
	if (m_old_motionflag != m_motionflag){
		m_motion[m_old_motionflag]->ResetTime();
		m_model->SetMotion(m_motion[m_motionflag]);
		m_old_motionflag = m_motionflag;
	}
	//アニメーションの時間を進める
	m_motion[m_motionflag]->AdvanceTime();
	//ボーンを更新する
	m_motion[m_motionflag]->UpdateBoneMatrix();
	D3DXVECTOR3 pos;
	////当たり判定（防御）
	//for (int i = 0; i < HIT_DEFENSE_POS; i++){
	//	if (g_HitDefense[i].flag){
	//		g_HitDefense[i].x = m_model->GetBoneAddress()->operator[](g_HitDefense[i].id).GetModelLocalPosition().x;
	//		g_HitDefense[i].y = m_model->GetBoneAddress()->operator[](g_HitDefense[i].id).GetModelLocalPosition().y;
	//		g_HitDefense[i].z = m_model->GetBoneAddress()->operator[](g_HitDefense[i].id).GetModelLocalPosition().z;
	//		pos = { g_HitDefense[i].x, g_HitDefense[i].y, g_HitDefense[i].z };
	//		HitBall->Update(g_HitDefense[i].world, m_MatWork, pos, g_HitDefense[i].radius);
	//	}
	//}
	////当たり判定（攻撃）
	//for (int i = 0; i < HIT_ATTACK_POS; i++){
	//	if (g_HitAttack[i].flag){
	//		g_HitAttack[i].x = m_model->GetBoneAddress()->operator[](g_HitAttack[i].id).GetModelLocalPosition().x;
	//		g_HitAttack[i].y = m_model->GetBoneAddress()->operator[](g_HitAttack[i].id).GetModelLocalPosition().y;
	//		g_HitAttack[i].z = m_model->GetBoneAddress()->operator[](g_HitAttack[i].id).GetModelLocalPosition().z;
	//		pos = { g_HitAttack[i].x, g_HitAttack[i].y, g_HitAttack[i].z };
	//		HitBall->Update(g_HitAttack[i].world, m_MatWork, pos, g_HitAttack[i].radius);
	//	}
	//}

	gauge->Move();
	if (gauge->Value <= 0 && gauge->PrevValue <= 0) {
		gauge->Reset();
	}
}

void CEnemy::Render()
{

	D3DLIGHT9 light;
	//m_lpd3ddevice->SetTransform(D3DTS_WORLD, &m_MatWork);
	m_model->Draw(m_MatWork, &light, camera->GetViewMatrix(), camera->GetProjectionMatrix());

	////当たり判定（防御）
	//for (int i = 0; i < HIT_DEFENSE_POS; i++){
	//	if (g_HitDefense[i].flag){
	//		HitBall->SetColor(D3DXCOLOR(0, 0, 1, 0.4f), D3DXCOLOR(0, 0, 0, 0));
	//		HitBall->Draw(g_HitDefense[i].world);
	//	}
	//}
	////当たり判定（攻撃）
	//for (int i = 0; i < HIT_ATTACK_POS; i++){
	//	if (g_HitAttack[i].flag){
	//		HitBall->SetColor(D3DXCOLOR(1, 0, 0, 0.4f), D3DXCOLOR(0, 0, 0, 0));
	//		HitBall->Draw(g_HitAttack[i].world);
	//	}
	//}
	gauge->Draw();
}

float CEnemy::GetRadius()
{
	//return g_HitDefense[g_HitDefense[0].id].radius;
	return false;
}

void CEnemy::LoadData()
{
	/*CLoadHitData* LoadDefense = new CLoadHitData("defense.txt");
	for (int i = 0; i < HIT_DEFENSE_POS; i++){
		hit_defense_name[i] = LoadDefense->LoadName();
		g_HitDefense[i].radius = LoadDefense->LoadRadius();
		LoadDefense->Reset();
	}
	CLoadHitData* LoadAttack = new CLoadHitData("attack.txt");
	for (int i = 0; i < HIT_ATTACK_POS; i++){
		hit_attack_name[i] = LoadAttack->LoadName();
		g_HitAttack[i].radius = LoadAttack->LoadRadius();
		LoadAttack->Reset();
	}
	SAFE_DELETE(LoadAttack);
	SAFE_DELETE(LoadDefense);*/
}

//入力検知
void CEnemy::InputJudge()
{
	CInput::Instance().UpdateState();
	const CInputState* is = CInput::Instance().GetState(0);
	int input = 0;
	input |= !is->Down&&!is->Up&&!is->Left&&!is->Right ? INPUT5 : INPUT_NOT5;
	input |= is->Down ? INPUT2 : INPUT_NOT2;
	input |= is->Up ? INPUT8 : INPUT_NOT8;
	input |= is->Button[0] ? INPUT_P : INPUT_NOT_P;
	input |= is->Button[1] ? INPUT_K : INPUT_NOT_K;
	input |= is->Button[2] ? INPUT_G : INPUT_NOT_G;
	if (Reverse) {
		input |= is->Left ? INPUT6 : INPUT_NOT6;
		input |= is->Right ? INPUT4 : INPUT_NOT4;
		input |= is->Left&&is->Down ? INPUT3 : INPUT_NOT3;
		input |= is->Right&&is->Down ? INPUT1 : INPUT_NOT1;
		input |= is->Left&&is->Up ? INPUT9 : INPUT_NOT9;
		input |= is->Right&&is->Up ? INPUT7 : INPUT_NOT7;
	}
	else {
		input |= is->Left ? INPUT4 : INPUT_NOT4;
		input |= is->Right ? INPUT6 : INPUT_NOT6;
		input |= is->Left&&is->Down ? INPUT1 : INPUT_NOT1;
		input |= is->Right&&is->Down ? INPUT3 : INPUT_NOT3;
		input |= is->Left&&is->Up ? INPUT7 : INPUT_NOT7;
		input |= is->Right&&is->Up ? INPUT9 : INPUT_NOT9;
	}
	//入力を１つ移動する
	for (int i = INPUT_COUNT - 1; i > 0; i--){
		m_nInput[i] = m_nInput[i - 1];
	}
	//入力を保存
	m_nInput[0] = input;
	m_debug->SetInput(input);
}

//コマンド判定
void CEnemy::CommandJudge()
{
	//CMD8
	if (CheckInput(CMD8)){
		m_debug->SetDeta("上", true);
		m_debug->SetId(0);
	}
	//CMD2
	else if (CheckInput(CMD2)){
		m_Speed = 0.0f;
		m_motionflag = DOWN;
		m_debug->SetDeta("下", true);
		m_debug->SetId(4);
	}
	//CMDP
	if (CheckInput(CMD5P)){
		m_motionflag = SMALL_PUNCH;
		gauge->Damage(0.1f);
		m_rigidity = 10;
		m_debug->SetDeta("パ", true);
	}
	//CMD9
	if (CheckInput(CMD9)){
		m_debug->SetDeta("右上", true);
		m_debug->SetId(1);
	}
	//CMD3
	else if (CheckInput(CMD3)){
		m_debug->SetDeta("右下", true);
		m_debug->SetId(3);
	}
	//CMD6
	else if (CheckInput(CMD6)){
		m_Speed = PLAYER_SPEED;
		m_motionflag = WALK;
		m_debug->SetDeta("右", true);
		m_debug->SetId(2);
	}
	//CMD7
	if (CheckInput(CMD7)){
		m_debug->SetDeta("左上", true);
		m_debug->SetId(7);
	}
	//CMD1
	else if (CheckInput(CMD1)){
		m_debug->SetDeta("左下", true);
		m_debug->SetId(5);
	}
	//CMD4
	else if (CheckInput(CMD4)){
		m_Speed = -PLAYER_SPEED;
		m_motionflag = WALK;
		m_debug->SetDeta("左", true);
		m_debug->SetId(6);
	}
	//CMD3236P
	if (CheckInput(CMD3236P)){
		//Shoot();
		m_debug->SetDeta("コマンド：3236P", true);
	}
}

// 回転移動量をセット（前回描画して以降の回転量）
void CEnemy::SetAngle(float x, float y, float z){
	m_Angle.x = x;
	m_Angle.y = y;
	m_Angle.z = z;
}

// 平行移動量をセット（前回描画して以降の移動量）
void CEnemy::SetTranslation(float x, float y, float z){
	m_Translation.x += x;
	m_Translation.y += y;
	m_Translation.z += z;
}

void CEnemy::MakeTotalMatrix()
{
	m_math->MakeTotalMatrix(m_MatWork, m_MatTotal, m_Angle, m_Translation);
}