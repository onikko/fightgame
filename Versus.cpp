#include "Versus.h"
#include "Input_macro.h"
#include "Utilities/SafeDelete.h"


#define THROW_DAMAGE 0.3f

typedef struct {
	LPCTSTR g_vmdname;
	bool loop_flag;
}VMDDATA;
VMDDATA g_VmdData[] = {
	{ "asset/motion/down.vmd", false },
	{ "asset/motion/stand_1.vmd", true },
	{ "asset/motion/walk_1.vmd", true },
	{ "asset/motion/êÇíºÉWÉÉÉìÉv.vmd", true },
	{ "asset/motion/pan.vmd", false },
	{ "asset/motion/kik.vmd", false },
	{ "asset/motion/ñ≈è∏ó≥ÉqÉbÉgèâìÆ.vmd", false }
};



SmartPointer<CSkinMeshEffect> g_Effect(new CSkinMeshEffect());


Play::Play(LPDIRECT3DDEVICE9 lpd3ddev, int CPUMode, int id) : m_lpd3ddevice(lpd3ddev), m_CPU(CPUMode), m_id(id)
{
	bool sts;
	if (id == 0){
		m_Angle = D3DXVECTOR3(0, -90, 0);
		m_Translation = D3DXVECTOR3(-1.8f, 0, 0.);
	}
	else{
		m_Angle = D3DXVECTOR3(0, 90, 0);
		m_Translation = D3DXVECTOR3(1.8f, 0, 0.);
	}
	m_model = new PmxSkinMesh();
	m_model->SetDevice(lpd3ddev);
	sts = m_model->LoadModel("asset/model/sakuya/sakuya(kari).pmx", lpd3ddev);
	if (!sts){
		MessageBox(NULL, "ERROR!!", "load model", MB_OK);
		return;
	}
	for (int i = 0; i < 7; i++){
		m_motion[i] = new VmdMotionController();
		m_motion[i]->LoadVmdFile(g_VmdData[i].g_vmdname, m_model->GetBoneAddress(), m_model->GetIkAddress(), g_VmdData[i].loop_flag);
	}
	g_Effect->LoadEffect("effect/skinmesh.fx");
	m_Reverse = (id == 1);
	printf("P1Çì«Ç›çûÇ›Ç‹ÇµÇΩ\n");
}

Play::~Play()
{
	SAFE_DELETE(m_model);
	for (int i = 0; i < 7; i++){
		SAFE_DELETE(m_motion[i]);
	}
	printf("P1Çîjä¸ÇµÇ‹ÇµÇΩ\n");
}

void Play::Init()
{
	D3DXMatrixIdentity(&m_MatTotal);
	D3DXMatrixIdentity(&m_MatWork);
	m_Speed = 0.0f;
	m_model->SetMotion(m_motion[0]);
	m_model->SetTech(g_Effect);
	MakeTotalMatrix();
}

void Play::Input()
{
	InputJudge();
}

void Play::Update ()
{

}

void Play::Render(D3DXMATRIX view, D3DXMATRIX projection)
{
	D3DLIGHT9 light;
	m_lpd3ddevice->SetTransform(D3DTS_WORLD, &m_MatWork);
	m_model->Draw(m_MatWork, &light, view, projection);
}


////////////////////////////////////////////////////////////////
// ì¸óÕÇÃîªíË
////////////////////////////////////////////////////////////////
bool Play::CheckInput(const int *command)
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

////////////////////////////////////////////////////////////////
//ì¸óÕåüím
////////////////////////////////////////////////////////////////
void Play::InputJudge()
{
	CInput::Instance().UpdateState();
	const CInputState* is = CInput::Instance().GetState(m_id);
	int input = 0;
	if (!m_CPU) {
		input |= !is->Down&&!is->Up&&!is->Left&&!is->Right ? INPUT5 : INPUT_NOT5;
		input |= is->Down ? INPUT2 : INPUT_NOT2;
		input |= is->Up ? INPUT8 : INPUT_NOT8;
		input |= is->Button[0] ? INPUT_P : INPUT_NOT_P;
		input |= is->Button[1] ? INPUT_K : INPUT_NOT_K;
		input |= is->Button[2] ? INPUT_G : INPUT_NOT_G;
		if (m_Reverse) {
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
	}
	//ì¸óÕÇÇPÇ¬à⁄ìÆÇ∑ÇÈ
	for (int i = INPUT_COUNT - 1; i > 0; i--){
		m_nInput[i] = m_nInput[i - 1];
	}
	//ì¸óÕÇï€ë∂
	m_nInput[0] = input;
}

////////////////////////////////////////////////////////////////
//ÉRÉ}ÉìÉhîªíË
////////////////////////////////////////////////////////////////
void Play::CommandJudge()
{
	//CMD8
	if (CheckInput(CMD8)){
	}
	//CMD2
	else if (CheckInput(CMD2)){
	}
	//CMDP
	if (CheckInput(CMD5P)){
	}
	//CMDK
	if (CheckInput(CMD5K)){
	}
	//CMD9
	if (CheckInput(CMD9)){
	}
	//CMD3
	else if (CheckInput(CMD3)){
	}
	//CMD6
	else if (CheckInput(CMD6)){
	}
	//CMD7
	if (CheckInput(CMD7)){
	}
	//CMD1
	else if (CheckInput(CMD1)){
	}
	//CMD4
	else if (CheckInput(CMD4)){
	}
	//CMD3236P
	if (CheckInput(CMD3236P)){
	}
}


void Play::MakeTotalMatrix()
{
	m_math->MakeTotalMatrix(m_MatWork, m_MatTotal, m_Angle, m_Translation);
}













Versus::Versus()
{
	m_play[0] = new Play(m_lpd3ddevice, 0, 0);
	m_play[1] = new Play(m_lpd3ddevice, 1, 1);
	/*for (int i = 0; i < 2; i++){
		m_player[i]->SetEnemyData(m_play[1 - i]);
	}*/
	m_CPUMode = 0;

	printf("VersusÇê∂ê¨ÇµÇ‹ÇµÇΩ\n");
}


Versus::~Versus()
{
	for (int i = 0; i < 2; i++){
		SAFE_DELETE(m_play[i]);
	}
	printf("VersusÇîjä¸ÇµÇ‹ÇµÇΩ\n");
}


void Versus::Init()
{
	camera = (CCamaraTPS*)FindItemBox("TPScamera");
	for (int i = 0; i < 2; i++){
		m_play[i]->Init();
	}
}

void Versus::Input()
{
	for (int i = 0; i < 2; i++){
		m_play[i]->Input();
	}
}

void Versus::Update()
{
	if (!m_UpdateFlag) { return; }
	for (int i = 0; i < 2; i++){ m_play[i]->Update(); }
	// í≤êÆÇÃèÄîı
	D3DXMATRIX& m0 = m_play[0]->GetMatrixWorld();
	D3DXMATRIX& m1 = m_play[1]->GetMatrixWorld();
	D3DXVECTOR3 p0 = D3DXVECTOR3(m0._41, 0, m0._43);
	D3DXVECTOR3 p1 = D3DXVECTOR3(m1._41, 0, m1._43);
	center = (p0 + p1)*0.5f;
	unit = p0 - p1;
	float dist = D3DXVec3Length(&unit);
	D3DXVec3Normalize(&unit, &unit);


	// çUåÇÇ∆ñhå‰
	for (int i = 0; i<2; i++) {
		Play* player = m_play[i];
		Play* enemy = m_play[1 - i];
		DEFENSE* defense = &Defense[enemy->AnimIndex];
		float time = 0;//player->GetAnimation()->Time*0.01f;
		for (int j = 0, jn = sizeof(Attack) / sizeof(ATTACK); j<jn; j++) {
			ATTACK* attack = &Attack[j];
			if (player->AttackValid[j] && attack->AnimIndex == player->AnimIndex &&
				attack->FromTime <= time && time <= attack->ToTime && CheckHit(i, attack->HitIndex)){
				player->AttackValid[j] = false;
				enemy->NextAnimIndex = defense->NextAnimIndex[attack->Position];
				enemy->WaitTime = attack->WaitTime * 100;
				if (Defense[enemy->NextAnimIndex].Attribute&DA_DAMAGE){
					//Gauge[1 - i]->Damage(attack->Damage);
				}
				if (defense->Attribute&DA_COUNTER){
					enemy->WaitTime *= (int)1.5f;
					if (Defense[enemy->NextAnimIndex].Attribute&DA_DAMAGE){
						//Gauge[1 - i]->Damage(attack->Damage*0.5f);
					}
					if (player->AnimIndex == 0 || player->AnimIndex == 0){
						enemy->NextAnimIndex = defense->NextAnimIndex[AP_BLOW];
					}
					//SEPlayer->Play(SECounterHit);
				}
				else {
					//SEPlayer->Play(SENormalHit);
				}
				break;
			}
		}
		if (player->Throw && dist <= 1*1.1f && (defense->Attribute&DA_THROWABLE) && (!enemy->Throw || (defense->Attribute&DA_THROWABLE) == 0)){
			player->NextAnimIndex = 0;
			enemy->NextAnimIndex = 0;
			//Gauge[1 - i]->Damage(THROW_DAMAGE);
		}
		if (player->DownAttack && enemy->AnimIndex == 0){
			player->NextAnimIndex = 0;
		}
	}

}

void Versus::Render()
{
	for (int i = 0; i < 2; i++){
		m_play[i]->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix());
	}
}


// ìñÇΩÇËîªíË
bool Versus::CheckHit(int attack_player_id, int attack_hit_id)
{
	HIT* hit_attack = (m_play[attack_player_id]->HalfBody ? HitAttackR : HitAttack) + attack_hit_id;
	D3DXMATRIX& am = m_play[attack_player_id]->GetModel()->GetBoneAddress()->operator[](0).GetModelLocalBoneMat();
	D3DXVECTOR3 apos(am._41, am._42, am._43);
	for (int i = 0, n = sizeof(HitDefense) / sizeof(HIT); i < n; i++){
		D3DXMATRIX& dm = m_play[1 - attack_player_id]->GetModel()->GetBoneAddress()->operator[](0).GetModelLocalBoneMat();
		D3DXVECTOR3 dpos(dm._41, dm._42, dm._43);
		D3DXVECTOR3 dist = apos - dpos;
		if (D3DXVec3Length(&dist) <= hit_attack->Radius + HitDefense[i].Radius){
			return true;
		}
	}
	return false;
}