#include "Versus.h"
#include "Input_macro.h"
#include "Utilities/SafeDelete.h"
#include "Rand.h"

#define THROW_DAMAGE 0.3f
#define STAGE_SIZE 7.5f
#define MIN_DIST 1.0f

typedef struct {
	LPCTSTR g_vmdname;
	bool loop_flag;
}VMDDATA;
VMDDATA g_VmdData[] = {
	{ "asset/motion/down.vmd", false },
	{ "asset/motion/stand_1.vmd", true },
	{ "asset/motion/walk_1.vmd", true },
	{ "asset/motion/sss.vmd", true },
	{ "asset/motion/pan.vmd", false },
	{ "asset/motion/kik.vmd", false },
	{ "asset/motion/滅昇竜ヒット初動.vmd", false }
};

CPU_ACTION CPUActionNear[] = {
	{ 1, CMD5PG, 0 },
	{ 1, CMD3236P, 0 },
	{ 1, CMD5PP, 0 },
	{ 1, CMD3K, 0 },
	{ 1, CMD2K, 0 },
	{ 1, CMD5K, 0 },
	{ 1, CMD2P, 0 },
	{ 1, CMD5P, 0 },
	{ 1, CMD121, 0 },
	{ 1, CMD44, 0 },
	{ 3, CMD4, 20 },
	{ 3, CMD6, 20 },
	{ 2, CMD2, 30 },
	{ 2, CMD5, 20 },
	{ 3, CMD2G, 20 },
	{ 3, CMD5G, 20 },
};
CPU_ACTION CPUActionFar[] = {
	{ 1, CMD5PP, 0 },
	{ 1, CMD5K, 0 },
	{ 1, CMD3K, 0 },
	{ 1, CMD2, 30 },
	{ 1, CMD5, 20 },
	{ 2, CMD323, 0 },
	{ 2, CMD66, 0 },
	{ 3, CMD4, 20 },
	{ 3, CMD6, 20 },
	{ 3, CMD2G, 20 },
	{ 3, CMD5G, 20 },
};

SmartPointer<CSkinMeshEffect> g_Effect(new CSkinMeshEffect());
CRand Rand;

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
Player::Player(LPDIRECT3DDEVICE9 lpd3ddev, int CPUMode, int id) : m_lpd3ddevice(lpd3ddev), m_CPU(CPUMode), m_id(id)
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
	// PMXモデル生成
	m_model = new PmxSkinMesh();
	m_model->SetDevice(lpd3ddev);
	sts = m_model->LoadModel("asset/model/sakuya/sakuya(kari).pmx", lpd3ddev);
	if (!sts){
		MessageBox(NULL, "ERROR!!", "load model", MB_OK);
		return;
	}
	// モーション生成

	/************        変更する        ***************/
	for (int i = 0; i < 7; i++){
		m_motion[i] = new VmdMotionController();
		m_motion[i]->LoadVmdFile(g_VmdData[i].g_vmdname, m_model->GetBoneAddress(), m_model->GetIkAddress(), g_VmdData[i].loop_flag);
	}
	/************        変更する        ***************/

	// エフェクトファイル生成
	g_Effect->LoadEffect("effect/skinmesh.fx");
	// ボーン名からIDを取得
	for (int i = 0; i < sizeof(HitDefense) / sizeof(HIT); i++){
		HitDefense[i].BoneIndex = m_model->GetBoneId(HitDefense[i].BoneName);
	}
	for (int i = 0; i < sizeof(HitAttack) / sizeof(HIT); i++){
		HitAttack[i].BoneIndex = m_model->GetBoneId(HitAttack[i].BoneName);
	}
	for (int i = 0; i < sizeof(HitAttackR) / sizeof(HIT); i++){
		HitAttackR[i].BoneIndex = m_model->GetBoneId(HitAttackR[i].BoneName);
	}


	CPU = false;
	CPUCommand = NULL;
	CPUCommandTime = 0;
	CPURepeat = 0;
	Throw = DownAttack = false;
	NextAnimIndex = -1;
	WaitTime = 0;
	HalfBody = false;
	m_animIndex = m_old_animIndex = 1;
	m_moveflag = false;
	m_Reverse = (id == 1);
	m_gauge = new CGauge(id);
	//Rand = new CRand();
	Rand.Init(1);
	m_id ? printf("P2を読み込みました\n") : printf("P1を読み込みました\n");
}

////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
Player::~Player()
{
	SAFE_DELETE(m_model);
	for (int i = 0; i < 7; i++){
		SAFE_DELETE(m_motion[i]);
	}
	SAFE_DELETE(m_gauge);
	m_id ? printf("P2を破棄しました\n") : printf("P1を破棄しました\n");
}

////////////////////////////////////////////////////////////////
// 初期化処理
////////////////////////////////////////////////////////////////
void Player::Init()
{
	D3DXMatrixIdentity(&m_MatTotal);
	D3DXMatrixIdentity(&m_MatWork);
	m_Speed = 0.0f;
	m_model->SetMotion(m_motion[1]);
	m_model->SetTech(g_Effect);
	MakeTotalMatrix();
}

////////////////////////////////////////////////////////////////
// 入力処理
////////////////////////////////////////////////////////////////
void Player::Input()
{
	m_Speed = 0;
	InputJudge();
	CommandJudge();
}

////////////////////////////////////////////////////////////////
// 更新処理
////////////////////////////////////////////////////////////////
void Player::Update()
{
	D3DXMATRIX scale;
	if (m_moveflag){
		m_Translation.x += m_Speed;
		//範囲外に行かないようにする
		if (m_Translation.x < -STAGE_SIZE){
			m_Translation.x += -STAGE_SIZE - m_Translation.x;
		}
		if (m_Translation.x > STAGE_SIZE){
			m_Translation.x += STAGE_SIZE - m_Translation.x;
		}
		MakeTotalMatrix();
	}
	else{
		m_Speed = 0;
		m_animIndex = 1;
	}
	//アニメーションが前回と違うと更新
	if (m_old_animIndex != m_animIndex){
		m_motion[m_old_animIndex]->ResetTime();
		m_model->SetMotion(m_motion[m_animIndex]);
		m_old_animIndex = m_animIndex;
	}
	//アニメーションの時間を進める
	m_motion[m_animIndex]->AdvanceTime();
	//ボーンを更新する
	m_motion[m_animIndex]->UpdateBoneMatrix();
}

////////////////////////////////////////////////////////////////
// 描画処理
////////////////////////////////////////////////////////////////
void Player::Render(D3DXMATRIX view, D3DXMATRIX projection)
{
	D3DLIGHT9 light;
	m_lpd3ddevice->SetTransform(D3DTS_WORLD, &m_MatWork);
	m_model->Draw(m_MatWork, &light, view, projection);
}


////////////////////////////////////////////////////////////////
// 入力の判定
////////////////////////////////////////////////////////////////
bool Player::CheckInput(const int *command)
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
//入力検知
////////////////////////////////////////////////////////////////
void Player::InputJudge()
{
	INPUT.UpdateState();
	const CInputState* is = INPUT.GetState(m_id);
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
	else {
		//コマンド入力中では無い時の処理
		if (CPUCommand == NULL){
			if (Enemy->AnimIndex == ANIM_DOWN0){
				CPUCommand = CMD_D8P;
				CPURepeat = 0;
			}
			else
				if (AnimIndex == ANIM_DOWN0){
					if (Rand.Real1() < 0.1f){
						CPUCommand = Rand.Real1() < 0.5f ? CMD5K : CMD2K;
					}
					else {
						CPUCommand = Rand.Real1()< 0.5f ? CMD5 : CMD2;
					}
					CPURepeat = 0;
				}
				else {
					D3DXVECTOR3 v = Enemy->GetTranslation() - m_Translation;
					float dist = D3DXVec3Length(&v);
					CPU_ACTION* action;
					int i, n, sum;
					if (dist <= MIN_DIST*1.5f){
						action = CPUActionNear;
						n = sizeof(CPUActionNear) / sizeof(CPU_ACTION);
					}
					else {
						action = CPUActionFar;
						n = sizeof(CPUActionFar) / sizeof(CPU_ACTION);
					}
					for (i = 0, sum = 0; i < n; i++){
						sum += (int)action[i].Rate;
					}
					int rate = (int)(Rand.Real1()*sum);
					for (i = 0, sum = 0; i < n - 1; i++){
						sum += (int)action[i].Rate;
						if (rate <= sum) break;
					}
					CPUCommand = action[i].Command;
					CPURepeat = (int)(Rand.Real1()*action[i].Repeat);
				}
				CPUCommandTime = 0;
		}
		else {
			if (CPUCommand[CPUCommandTime] & INPUT_END){
				CPUCommandTime = 0;
				if (CPURepeat > 0){
					CPURepeat--;
				}
				else{
					CPUCommand = NULL;
				}
			}
			if (CPUCommand && !(CPUCommand[CPUCommandTime] & INPUT_END)){
				input |= CPUCommand[CPUCommandTime];
				CPUCommandTime++;
			}
		}
	}
	//入力を１つ移動する
	for (int i = INPUT_COUNT - 1; i > 0; i--){
		m_nInput[i] = m_nInput[i - 1];
	}
	//入力を保存
	m_nInput[0] = input;
}

////////////////////////////////////////////////////////////////
//コマンド判定
////////////////////////////////////////////////////////////////
void Player::CommandJudge()
{
	//CMD8
	if (CheckInput(CMD8)){
		m_animIndex = 3;
	}
	//CMD2
	else if (CheckInput(CMD2)){
		m_animIndex = 0;
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
		m_Speed = 0.02f;
		m_animIndex = 2;
	}

	//CMD7
	if (CheckInput(CMD7)){
	}
	//CMD1
	else if (CheckInput(CMD1)){
	}
	//CMD4
	else if (CheckInput(CMD4)){
		m_Speed = -0.02f;
		m_animIndex = 2;
	}

	//CMD3236P
	if (CheckInput(CMD3236P)){
	}

	if (CheckInput(CMD2G)){
		int p;
		p = 0;
	}
	if (CheckInput(CMD5G)){
		m_Speed = -0.02f;
		m_animIndex = 2;
	}

	//入力がなければ
	m_moveflag = CheckInput(CMD5) ? false : true;

}

////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////
void Player::MakeTotalMatrix()
{
	m_math->MakeTotalMatrix(m_MatWork, m_MatTotal, m_Angle, m_Translation);
}

////////////////////////////////////////////////////////////////
// 回転移動量をセット（前回描画して以降の回転量）
////////////////////////////////////////////////////////////////
void Player::SetAngle(float x, float y, float z){
	m_Angle.x = x;
	m_Angle.y = y;
	m_Angle.z = z;
}

////////////////////////////////////////////////////////////////
// 平行移動量をセット（前回描画して以降の移動量）
////////////////////////////////////////////////////////////////
void Player::SetTranslation(float x, float y, float z){
	m_Translation.x += x;
	m_Translation.y += y;
	m_Translation.z += z;
}





//********************************************************************************************
// Versus
//********************************************************************************************

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
Versus::Versus()
{
	for (int i = 0; i < 2; i++){
		m_player[i] = new Player(m_lpd3ddevice, i, i);
	}
	for (int i = 0; i < 2; i++){
		m_player[i]->Enemy = m_player[1 - i];
	}
	m_HitBall = new CHitBox("asset/model/Ball.x");
	m_CPUMode = 0;
	m_HitMode = 1;
	printf("Versusを生成しました\n");
}

////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
Versus::~Versus()
{
	for (int i = 0; i < 2; i++){
		SAFE_DELETE(m_player[i]);
	}
	SAFE_DELETE(m_HitBall);
	printf("Versusを破棄しました\n");
}

////////////////////////////////////////////////////////////////
// 初期化処理
////////////////////////////////////////////////////////////////
void Versus::Init()
{
	camera = (CCamaraTPS*)FindItemBox("TPScamera");
	for (int i = 0; i < 2; i++){
		m_player[i]->Init();
	}
}

////////////////////////////////////////////////////////////////
// 入力処理
////////////////////////////////////////////////////////////////
void Versus::Input()
{
	INPUT.UpdateState();

	// 一時停止・コマ送りなど
	const CInputState* is = INPUT.GetState(0);
	if (!m_PrevInput && is->Button[3]) m_Paused = !m_Paused;
	bool move = !m_Paused || (!m_PrevInput && is->Button[4]);
	if (!m_PrevInput && is->Button[5]) m_HitMode = (m_HitMode + 1) % 3;
	if (!m_PrevInput && is->Button[6]) m_CPUMode = (m_CPUMode + 1) % 4;
	m_player[0]->CPU = (m_CPUMode == 2 || m_CPUMode == 3);
	m_player[1]->CPU = (m_CPUMode == 1 || m_CPUMode == 3);
	m_PrevInput = false;
	for (int i = 0; i < 2; i++){
		m_player[i]->Input();
	}
}

////////////////////////////////////////////////////////////////
// 更新処理
////////////////////////////////////////////////////////////////
void Versus::Update()
{
	if (!m_UpdateFlag){
		return;
	}
	for (int i = 0; i < 2; i++){
		m_player[i]->Update();
	}
	// 調整の準備
	D3DXMATRIX& m0 = m_player[0]->GetMatrixWorld();
	D3DXMATRIX& m1 = m_player[1]->GetMatrixWorld();
	D3DXVECTOR3 p0 = D3DXVECTOR3(m0._41, 0, m0._43);
	D3DXVECTOR3 p1 = D3DXVECTOR3(m1._41, 0, m1._43);
	center = (p0 + p1)*0.5f;
	unit = p0 - p1;
	dist = D3DXVec3Length(&unit);
	D3DXVec3Normalize(&unit, &unit);


	// 攻撃と防御
	for (int i = 0; i<2; i++) {
		Player* player = m_player[i];
		Player* enemy = m_player[1 - i];
		DEFENSE* defense = &Defense[enemy->AnimIndex];
		float time = player->GetAnimation()*0.01f;
		for (int j = 0, jn = sizeof(Attack) / sizeof(ATTACK); j<jn; j++) {
			ATTACK* attack = &Attack[j];
			if (player->AttackValid[j] && attack->AnimIndex == player->AnimIndex &&
				attack->FromTime <= time && time <= attack->ToTime && CheckHit(i, attack->HitIndex)){
				player->AttackValid[j] = false;
				enemy->NextAnimIndex = defense->NextAnimIndex[attack->Position];
				enemy->WaitTime = attack->WaitTime * 100;
				if (Defense[enemy->NextAnimIndex].Attribute&DA_DAMAGE){
					m_player[i]->GetGauge()->Damage(attack->Damage);
				}
				if (defense->Attribute&DA_COUNTER){
					enemy->WaitTime *= (int)1.5f;
					if (Defense[enemy->NextAnimIndex].Attribute&DA_DAMAGE){
						m_player[i]->GetGauge()->Damage(attack->Damage*0.5f);
					}
					if (player->AnimIndex == ANIM_3236P || player->AnimIndex == ANIM_5K){
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
		if (player->Throw && dist <= MIN_DIST*1.1f && (defense->Attribute&DA_THROWABLE) && (!enemy->Throw || (defense->Attribute&DA_THROWABLE) == 0)){
			player->NextAnimIndex = 0;
			enemy->NextAnimIndex = 0;
			m_player[i]->GetGauge()->Damage(THROW_DAMAGE);
		}
		if (player->DownAttack && enemy->AnimIndex == 0){
			player->NextAnimIndex = 0;
		}
	}

	// 調整
	if (dist < MIN_DIST){
		D3DXVECTOR3 vpos = unit*(MIN_DIST - dist)*0.5f;
		m_player[0]->SetTranslation(vpos.x, vpos.y, vpos.z);
		m_player[1]->SetTranslation(-vpos.x, -vpos.y, -vpos.z);
	}

	for (int i = 0; i < 2; i++){
		m_player[i]->MakeTotalMatrix();
	}

	// 入力方向の調整
	D3DXVECTOR4 v[2];
	for (int i = 0; i<2; i++) {
		D3DXVec3Transform(&v[i], &m_player[i]->GetTranslation(), &camera->GetViewMatrix());
	}
	for (int i = 0; i<2; i++) {
		m_player[i]->m_Reverse = (v[i].x>v[1 - i].x);
	}

	// ゲージ
	for (int i = 0; i<2; i++) {
		m_player[i]->GetGauge()->Move();
		if (m_player[i]->GetGauge()->Value <= 0 && m_player[i]->GetGauge()->PrevValue <= 0) {
			m_player[1 - i]->GetGauge()->WinCount++;
			m_player[i]->GetGauge()->Reset();
		}
	}
}

////////////////////////////////////////////////////////////////
// 描画処理
////////////////////////////////////////////////////////////////
void Versus::Render()
{
	for (int i = 0; i < 2; i++){
		// キャラ
		m_player[i]->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix());
		// ゲージ
		m_player[i]->GetGauge()->Draw();
	}
	// 当たり判定
	if (m_HitMode>0){
		for (int i = 0; i<2; i++){
			HIT* hit;
			int n;
			if ((m_HitMode + i) % 2 == 0){
				hit = HitDefense;
				n = sizeof(HitDefense) / sizeof(HIT);
			}
			else{
				hit = HitAttack;
				n = sizeof(HitAttack) / sizeof(HIT);
			}
			for (int j = 0; j<n; j++){
				D3DXVECTOR3 pos;
				D3DXMATRIX scale;
				D3DXMATRIX world;
				pos.x = m_player[i]->GetModel()->GetBoneAddress()->operator[](hit[j].BoneIndex).GetModelLocalPosition().x;
				pos.y = m_player[i]->GetModel()->GetBoneAddress()->operator[](hit[j].BoneIndex).GetModelLocalPosition().y;
				pos.z = m_player[i]->GetModel()->GetBoneAddress()->operator[](hit[j].BoneIndex).GetModelLocalPosition().z;
				m_HitBall->Update(world, m_player[i]->GetMatrixWorld(), pos, hit[j].Radius);
				if (hit == HitAttack){
					if (CheckHit(i, j)){
						m_HitBall->SetColor(D3DXCOLOR(0.2f, 0, 0, 0.8f), D3DXCOLOR(0, 0, 0, 0));
					}
					else{
						m_HitBall->SetColor(D3DXCOLOR(0, 0.5f, 0, 0.6f), D3DXCOLOR(0, 0, 0, 0));
					}
				}
				else{
					m_HitBall->SetColor(D3DXCOLOR(0, 0, 1, 0.4f), D3DXCOLOR(0, 0, 0, 0));
				}
				m_HitBall->Draw(world);
			}
		}
	}
}


////////////////////////////////////////////////////////////////
// 当たり判定
////////////////////////////////////////////////////////////////
bool Versus::CheckHit(int attack_player_id, int attack_hit_id)
{
	HIT* hit_attack = (m_player[attack_player_id]->HalfBody ? HitAttackR : HitAttack) + attack_hit_id;
	D3DXVECTOR3 pos;
	pos.x = m_player[attack_player_id]->GetModel()->GetBoneAddress()->operator[](hit_attack->BoneIndex).GetModelLocalPosition().x;
	pos.y = m_player[attack_player_id]->GetModel()->GetBoneAddress()->operator[](hit_attack->BoneIndex).GetModelLocalPosition().y;
	pos.z = m_player[attack_player_id]->GetModel()->GetBoneAddress()->operator[](hit_attack->BoneIndex).GetModelLocalPosition().z;
	D3DXVECTOR3 m_wpos;
	D3DXVec3TransformCoord(&m_wpos, &pos, &m_player[attack_player_id]->GetMatrixWorld());
	D3DXMATRIX& am = m_player[attack_player_id]->GetMatrixWorld();
	am._41 = m_wpos.x;
	am._42 = m_wpos.y;
	am._43 = m_wpos.z;
	D3DXVECTOR3 apos(am._41, am._42, am._43);
	for (int i = 0, n = sizeof(HitDefense) / sizeof(HIT); i < n; i++){
		pos.x = m_player[1 - attack_player_id]->GetModel()->GetBoneAddress()->operator[](HitDefense[i].BoneIndex).GetModelLocalPosition().x;
		pos.y = m_player[1 - attack_player_id]->GetModel()->GetBoneAddress()->operator[](HitDefense[i].BoneIndex).GetModelLocalPosition().y;
		pos.z = m_player[1 - attack_player_id]->GetModel()->GetBoneAddress()->operator[](HitDefense[i].BoneIndex).GetModelLocalPosition().z;
		D3DXVec3TransformCoord(&m_wpos, &pos, &m_player[1 - attack_player_id]->GetMatrixWorld());
		D3DXMATRIX& dm = m_player[1 - attack_player_id]->GetMatrixWorld();
		dm._41 = m_wpos.x;
		dm._42 = m_wpos.y;
		dm._43 = m_wpos.z;
		D3DXVECTOR3 dpos(dm._41, dm._42, dm._43);
		D3DXVECTOR3 dist = apos - dpos;
		if (D3DXVec3Length(&dist) <= hit_attack->Radius + HitDefense[i].Radius){
			return true;
		}
	}
	return false;
}