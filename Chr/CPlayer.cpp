#include "CPlayer.h"
#include "../Utilities/SafeDelete.h"
#include "../HitBox.h"
#include "../Input_macro.h"

#include "../Scene/SceneMgr.h"
#include "../Render.h"
#include "../LoadHitData.h"

#define PLAYER_SPEED (0.02f)

CHitBox* g_HitBall;
SmartPointer<CSkinMeshEffect> effect(new CSkinMeshEffect());

string g_hit_attack_name[HIT_ATTACK_POS];
string g_hit_defense_name[HIT_DEFENSE_POS];

typedef struct {
	LPCTSTR g_vmdname;
	bool loop_flag;
}VMDDATA;
VMDDATA g_vmddata[] = {
	{ "asset/motion/down.vmd", false },
	{ "asset/motion/stand_1.vmd", true},
	{ "asset/motion/walk_1.vmd" , true },
	{ "asset/motion/�����W�����v.vmd", true },
	{ "asset/motion/pan.vmd", false },
	{ "asset/motion/kik.vmd", false },
	{ "asset/motion/�ŏ����q�b�g����.vmd", false }
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
}_ANIM;


struct MOTION {
	int AnimIndex;
	const int* Command;
};

MOTION Motion[] = {
	{ SMALL_KIKKU, CMD5K },
	{ SMALL_PUNCH, CMD5P },
	{ DOWN, CMD2 },
	{ WALK, CMD4 },
	{ WALK, CMD6 },
	{ STAND, CMD5 },
};

// �J�X�^�����_
struct Vertex {
	D3DXVECTOR3 pos;
	D3DCOLOR diffuse;
	float u, v;
};

#define		VERTEX_FVF	(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

// �r���{�[�h���_
Vertex billboard_g[4] = {
	{ D3DXVECTOR3(-0.5f, -0.5f, -0.0f), D3DCOLOR_ARGB(100, 0, 255, 0), 0.0f, 1.0f },
	{ D3DXVECTOR3(-0.5f, 0.5f, -0.0f), D3DCOLOR_ARGB(100, 0, 255, 0), 0.0f, 0.0f },
	{ D3DXVECTOR3(0.5f, -0.5f, -0.0f), D3DCOLOR_ARGB(100, 0, 255, 0), 1.0f, 1.0f },
	{ D3DXVECTOR3(0.5f, 0.5f, -0.0f), D3DCOLOR_ARGB(100, 0, 255, 0), 1.0f, 0.0f }
};

IDirect3DVertexBuffer9 *buffer = 0;

//--------------------------------------------------------------
// CPlayer
//--------------------------------------------------------------
CPlayer::CPlayer(D3DXVECTOR3 angle, D3DXVECTOR3 translation, bool moveflag, LPDIRECT3DDEVICE9 lpd3ddev)
{
	m_Angle = { angle.x, angle.y, angle.z };
	m_Translation = { translation.x, translation.y, translation.z };
	m_jumpflag = 0;
	m_animIndex = m_old_animIndex = STAND;
	m_rigidity = 0;
	m_moveflag = moveflag;
	m_Speed = 0.0f;
	Reverse = false;
	id = 0;
	damege = 0.0f;
	for (int i = 0; i<INPUT_COUNT; i++) m_nInput[i] = INPUT_NULL;
	bool sts;
	//CGeneralFactory<PmxSkinMesh>::Instance().Create(0, 10, m_model);
	m_model = new PmxSkinMesh();
	m_model->SetDevice(lpd3ddev);
	sts = m_model->LoadModel("asset/model/sakuya/sakuya(kari).pmx", lpd3ddev);
	//sts = m_model->LoadModel("model/�쒀�͓V�Õ�1.1/�V�Õ��i�~���Ȃ��j.pmx", m_lpd3ddevice);
	if (!sts){
		MessageBox(NULL, "ERROR!!", "load model", MB_OK);
		return;
	}

	//�f�[�^���[�h
	LoadData();

	////�����蔻��i�h��j
	//for (int i = 0; i < HIT_DEFENSE_POS; i++){
	//	HitDefense[i].id = m_model->GetBoneId(g_hit_defense_name[i]);
	//	HitDefense[i].x = m_model->GetBoneAddress()->operator[](HitDefense[i].id).GetModelLocalPosition().x;
	//	HitDefense[i].y = m_model->GetBoneAddress()->operator[](HitDefense[i].id).GetModelLocalPosition().y;
	//	HitDefense[i].z = m_model->GetBoneAddress()->operator[](HitDefense[i].id).GetModelLocalPosition().z;
	//	D3DXMatrixIdentity(&HitDefense[i].world);
	//	HitDefense[i].flag = true;
	//}
	////�����蔻��i�U���j
	//for (int i = 0; i < HIT_ATTACK_POS; i++){
	//	HitAttack[i].id = m_model->GetBoneId(g_hit_attack_name[i]);
	//	HitAttack[i].x = m_model->GetBoneAddress()->operator[](HitAttack[i].id).GetModelLocalPosition().x;
	//	HitAttack[i].y = m_model->GetBoneAddress()->operator[](HitAttack[i].id).GetModelLocalPosition().y;
	//	HitAttack[i].z = m_model->GetBoneAddress()->operator[](HitAttack[i].id).GetModelLocalPosition().z;
	//	D3DXMatrixIdentity(&HitAttack[i].world);
	//	HitAttack[i].flag = true;
	//}

	for (int i = 0; i < ANIM_MAX; i++){
		//CGeneralFactory<VmdMotionController>::Instance().Create(0, i, m_motion[i]);
		m_motion[i] = new VmdMotionController();
		m_motion[i]->LoadVmdFile(g_vmddata[i].g_vmdname, m_model->GetBoneAddress(), m_model->GetIkAddress(), g_vmddata[i].loop_flag);
	}
	effect->LoadEffect("effect/skinmesh.fx");
	gauge = new CGauge(0);
	g_HitBall = new CHitBox(_T("asset/model/Ball.x"));
	m_lpd3ddevice = lpd3ddev;
	printf("P1��ǂݍ��݂܂���\n");
}


CPlayer::~CPlayer()
{
	SAFE_DELETE(m_model);
	for (int i = 0; i < ANIM_MAX; i++){
		SAFE_DELETE(m_motion[i]);
	}
	SAFE_DELETE(g_HitBall);
	SAFE_DELETE(gauge);
	printf("P1��j�����܂���\n");
}

void CPlayer::Init()
{
	//m_debug = (CDebugMgr*)FindItemBox("Debug");
	D3DXMatrixIdentity(&m_MatTotal);
	D3DXMatrixIdentity(&m_MatWork);
	m_Speed = PLAYER_SPEED;
	m_model->SetMotion(m_motion[STAND]);
	m_model->SetTech(effect);
	//camera->GetViewMatrix();
	MakeTotalMatrix();

	//// �|���S���쐬
	//if (FAILED(m_lpd3ddevice->CreateVertexBuffer(sizeof(Vertex) * 4, 0, VERTEX_FVF, D3DPOOL_MANAGED, &buffer, 0))) {
	//	SAFE_RELEASE(m_lpd3ddevice);
	//	SAFE_RELEASE(m_lpd3d);
	//	return;
	//}

	//Vertex *p;
	//buffer->Lock(0, 0, (void**)&p, 0);
	//{
	//	memcpy(p, billboard_g, sizeof(Vertex) * 4);
	//	buffer->Unlock();
	//}

	D3DXMatrixIdentity(&worldMat);
}

// ���͂̔���
bool CPlayer::CheckInput(const int *command)
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


void CPlayer::Input()
{

	CInput::Instance().UpdateState();
	const CInputState* is = CInput::Instance().GetState(0);
	//if (!m_PrevInput && is->Button[3]) m_Paused = !m_Paused;
	//m_UpdateFlag = !m_Paused || (!m_PrevInput && is->Button[4]);
	//if (!PrevInput && is->Button[5]) HitMode = (HitMode + 1) % 3;
	//if (!m_PrevInput && is->Button[7]) ShaderEnabled = !ShaderEnabled;
	//if (!m_PrevInput && is->Button[9]) m_TopView = !m_TopView;
	//m_PrevInput = false;
	//for (int i = 3; i <= 9; i++) m_PrevInput |= is->Button[i];

	InputJudge();

	if (m_rigidity == 0){
		CommandJudge();
	}
	//���͂��Ȃ����
	m_moveflag = CheckInput(CMD5) && m_rigidity == 0 ? false : true;

	//�f�o�b�O
	if (CInput::Instance().CheckKeyBufferTrigger(DIK_F4)){
		LoadData();
	}
	/*if (CInput::Instance().CheckKeyBufferTrigger(DIK_F5)){
		for (int i = 0; i < HIT_DEFENSE_POS; i++){
			HitDefense[i].flag = false;
		}
		for (int i = 0; i < HIT_ATTACK_POS; i++){
			HitAttack[i].flag = false;
		}
	}
	if (CInput::Instance().CheckKeyBufferTrigger(DIK_F6)){
		for (int i = 0; i < HIT_DEFENSE_POS; i++){
			HitDefense[i].flag = true;
		}
		for (int i = 0; i < HIT_ATTACK_POS; i++){
			HitAttack[i].flag = true;
		}
	}*/

	/*if (CInput::Instance().CheckKeyBufferTrigger(DIK_SPACE)){
		RemoveObj(1, INT_MAX - 1);
		AppendObj(new CTitle(), 1, true);
	}*/

}

void CPlayer::Update()
{
	//if (!m_UpdateFlag) return;

	if (m_rigidity != 0){
		m_Speed = 0.0f;
	}
	else{
		id = 0;
		damege = 0.0f;
	}
	if (m_moveflag){
		m_Translation.x += m_Speed;
		if (m_jumpflag == 1){
			m_Translation.y += 0.1f;
			m_animIndex = JUMP;
			if (m_MatTotal._42 > 1.0f){
				m_Translation.y = 1.0f;
				m_jumpflag = 2;
			}
		}
		if (m_jumpflag == 2){
			m_Translation.y -= 0.1f;
			m_animIndex = JUMP;
			if (m_MatTotal._42 < 0.0f){
				m_Translation.y = 0.0f;
				m_jumpflag = 0;
			}
		}
		if (m_Translation.x < -7.5f){
			m_Translation.x += -7.5f - m_Translation.x;
		}
		if (m_Translation.x > 7.5f){
			m_Translation.x += 7.5f - m_Translation.x;
		}
		MakeTotalMatrix();
		m_rigidity--;
	}
	else{
		m_Speed = 0;
		m_animIndex = STAND;
	}

	if (m_rigidity < 0){
		m_rigidity = 0;
	}
	if (m_old_animIndex != m_animIndex){
		m_motion[m_old_animIndex]->ResetTime();
		m_model->SetMotion(m_motion[m_animIndex]);
		m_old_animIndex = m_animIndex;
	}
	//�A�j���[�V�����̎��Ԃ�i�߂�
	m_motion[m_animIndex]->AdvanceTime();
	//�{�[�����X�V����
	m_motion[m_animIndex]->UpdateBoneMatrix();
	D3DXVECTOR3 pos;
	////�����蔻��i�h��j
	//for (int i = 0; i < HIT_DEFENSE_POS; i++){
	//	if (HitDefense[i].flag){
	//		HitDefense[i].x = m_model->GetBoneAddress()->operator[](HitDefense[i].id).GetModelLocalPosition().x;
	//		HitDefense[i].y = m_model->GetBoneAddress()->operator[](HitDefense[i].id).GetModelLocalPosition().y;
	//		HitDefense[i].z = m_model->GetBoneAddress()->operator[](HitDefense[i].id).GetModelLocalPosition().z;
	//		pos = { HitDefense[i].x, HitDefense[i].y, HitDefense[i].z };
	//		g_HitBall->Update(HitDefense[i].world, m_MatWork, pos, HitDefense[i].radius);
	//	}
	//}
	////�����蔻��i�U���j
	//for (int i = 0; i < HIT_ATTACK_POS; i++){
	//	if (HitAttack[i].flag){
	//		HitAttack[i].x = m_model->GetBoneAddress()->operator[](HitAttack[i].id).GetModelLocalPosition().x;
	//		HitAttack[i].y = m_model->GetBoneAddress()->operator[](HitAttack[i].id).GetModelLocalPosition().y;
	//		HitAttack[i].z = m_model->GetBoneAddress()->operator[](HitAttack[i].id).GetModelLocalPosition().z;
	//		pos = { HitAttack[i].x, HitAttack[i].y, HitAttack[i].z };
	//		g_HitBall->Update(HitAttack[i].world, m_MatWork, pos, HitAttack[i].radius);
	//	}
	//}

	gauge->Move();
	if (gauge->Value <= 0 && gauge->PrevValue <= 0) {
		gauge->Reset();
	}
}

void CPlayer::Render(D3DXMATRIX view, D3DXMATRIX projection)
{

	D3DLIGHT9 light;
	m_lpd3ddevice->SetTransform(D3DTS_WORLD, &m_MatWork);
	m_model->Draw(m_MatWork, &light, view, projection);

	

	////�����蔻��i�h��j
	//for (int i = 0; i < HIT_DEFENSE_POS; i++){
	//	if (HitDefense[i].flag){
	//		g_HitBall->SetColor(D3DXCOLOR(0, 0, 1, 0.4f), D3DXCOLOR(0, 0, 0, 0));
	//		g_HitBall->Draw(HitDefense[i].world);
	//	}
	//}
	////�����蔻��i�U���j
	//for (int i = 0; i < HIT_ATTACK_POS; i++){
	//	if (HitAttack[i].flag){
	//		g_HitBall->SetColor(D3DXCOLOR(1, 0, 0, 0.4f), D3DXCOLOR(0, 0, 0, 0));
	//		g_HitBall->Draw(HitAttack[i].world);
	//	}
	//}
	gauge->Draw();
}

float CPlayer::GetRadius()
{
	//return HitDefense[HitDefense[0].id].radius;
	return false;
}

void CPlayer::LoadData()
{
	/*CLoadHitData* LoadDefense = new CLoadHitData("defense.txt");
	for (int i = 0; i < HIT_DEFENSE_POS; i++){
		g_hit_defense_name[i] = LoadDefense->LoadName();
		HitDefense[i].radius = LoadDefense->LoadRadius();
		LoadDefense->Reset();
	}
	CLoadHitData* LoadAttack = new CLoadHitData("attack.txt");
	for (int i = 0; i < HIT_ATTACK_POS; i++){
		g_hit_attack_name[i] = LoadAttack->LoadName();
		HitAttack[i].radius = LoadAttack->LoadRadius();
		LoadAttack->Reset();
	}
	SAFE_DELETE(LoadAttack);
	SAFE_DELETE(LoadDefense);*/
}

//���͌��m
void CPlayer::InputJudge()
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
	//���͂��P�ړ�����
	for (int i = INPUT_COUNT - 1; i > 0; i--){
		m_nInput[i] = m_nInput[i - 1];
	}
	//���͂�ۑ�
	m_nInput[0] = input;
	//m_debug->SetInput(input);
}

//�R�}���h����
void CPlayer::CommandJudge()
{
	//CMD8
	if (CheckInput(CMD8)){
		//m_debug->SetDeta("��", true);
		//m_debug->SetId(0);
	}
	//CMD2
	else if (CheckInput(CMD2)){
		m_Speed = 0.0f;
		m_animIndex = DOWN;
		//m_debug->SetDeta("��", true);
		//m_debug->SetId(4);
	}
	//CMDP
	if (CheckInput(CMD5P)){
		m_animIndex = SMALL_PUNCH;
		m_rigidity = 10;
		//m_debug->SetDeta("�p", true);
		id = 0;
		damege = 0.1f;
	}
	//CMDK
	if (CheckInput(CMD5K)){
		m_animIndex = SMALL_KIKKU;
		m_rigidity = 10;
		//m_debug->SetDeta("�L", true);
		id = 3;
		damege = 0.3f;
	}
	//CMD9
	if (CheckInput(CMD9)){
		//m_debug->SetDeta("�E��", true);
		//m_debug->SetId(1);
	}
	//CMD3
	else if (CheckInput(CMD3)){
		//m_debug->SetDeta("�E��", true);
		//m_debug->SetId(3);
	}
	//CMD6
	else if (CheckInput(CMD6)){
		m_Speed = PLAYER_SPEED;
		m_animIndex = WALK;
		//m_debug->SetDeta("�E", true);
		//m_debug->SetId(2);
	}
	//CMD7
	if (CheckInput(CMD7)){
		//m_debug->SetDeta("����", true);
		//m_debug->SetId(7);
	}
	//CMD1
	else if (CheckInput(CMD1)){
		//m_debug->SetDeta("����", true);
		//m_debug->SetId(5);
	}
	//CMD4
	else if (CheckInput(CMD4)){
		m_Speed = -PLAYER_SPEED;
		m_animIndex = WALK;
		//m_debug->SetDeta("��", true);
		//m_debug->SetId(6);
	}
	//CMD3236P
	if (CheckInput(CMD3236P)){
		m_animIndex = SAMPLE;
		m_rigidity = 150;
		id = 1;
		damege = 0.3f;
		//m_debug->SetDeta("�R�}���h�F3236P", true);
	}
}

// ��]�ړ��ʂ��Z�b�g�i�O��`�悵�Ĉȍ~�̉�]�ʁj
void CPlayer::SetAngle(float x, float y, float z){
	m_Angle.x = x;
	m_Angle.y = y;
	m_Angle.z = z;
}

// ���s�ړ��ʂ��Z�b�g�i�O��`�悵�Ĉȍ~�̈ړ��ʁj
void CPlayer::SetTranslation(float x, float y, float z){
	m_Translation.x += x;
	m_Translation.y += y;
	m_Translation.z += z;
}

void CPlayer::MakeTotalMatrix()
{
	m_math->MakeTotalMatrix(m_MatWork, m_MatTotal, m_Angle, m_Translation);
}

//void CPlayer::Sample()
//{
//	D3DXVECTOR3 m_wpos;
//	D3DXVECTOR3 pos;
//	D3DXMATRIX	maty;
//	D3DXMatrixIdentity(&worldMatsample);
//	pos = { 0.0f, 0.0f, 0.0f };
//	D3DXVec3TransformCoord(&m_wpos, &pos, &worldMatsample);
//	D3DXMatrixRotationY(&maty, (float)(90 / 180 * D3DX_PI));
//	worldMatsample = maty;
//	worldMatsample._11 = 0.5f;
//	worldMatsample._41 = m_wpos.x;
//	worldMatsample._42 = m_wpos.y;
//	worldMatsample._43 = 0.0f;
//
//	m_lpd3ddevice->SetRenderState(D3DRS_LIGHTING, FALSE);
//	m_lpd3ddevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
//
//	// �s��ݒ�
//	m_lpd3ddevice->SetTransform(D3DTS_WORLD, &worldMatsample);
//
//	// �|����`��
//	m_lpd3ddevice->SetTexture(0, NULL);
//	m_lpd3ddevice->SetStreamSource(0, buffer, 0, sizeof(Vertex));
//	m_lpd3ddevice->SetFVF(VERTEX_FVF);
//	m_lpd3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
//
//
//	pos = { HitDefense[1].x, HitDefense[1].y, HitDefense[1].z };
//	D3DXVec3TransformCoord(&m_wpos, &pos, &m_MatWork);
//	D3DXMatrixRotationY(&maty, (float)(90 / 180 * D3DX_PI));
//	worldMat = maty;
//	worldMat._11 = 0.5f;
//	worldMat._41 = m_wpos.x;
//	worldMat._42 = m_wpos.y;
//	worldMat._43 = 0.0f;
//
//	m_lpd3ddevice->SetRenderState(D3DRS_LIGHTING, FALSE);
//	m_lpd3ddevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
//	for (int i = 0; i < 4; i++){
//		billboard_g[i].diffuse = D3DCOLOR_ARGB(100, 0, 255, 0);
//	}
//	Vertex *p;
//	buffer->Lock(0, 0, (void**)&p, 0);
//	{
//		memcpy(p, billboard_g, sizeof(Vertex) * 4);
//		buffer->Unlock();
//	}
//
//	// �s��ݒ�
//	m_lpd3ddevice->SetTransform(D3DTS_WORLD, &worldMat);
//
//	// �|����`��
//	m_lpd3ddevice->SetTexture(0, NULL);
//	m_lpd3ddevice->SetStreamSource(0, buffer, 0, sizeof(Vertex));
//	m_lpd3ddevice->SetFVF(VERTEX_FVF);
//	m_lpd3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
//
//
//	m_lpd3ddevice->SetRenderState(D3DRS_LIGHTING, TRUE);
//	m_lpd3ddevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
//}