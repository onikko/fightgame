#include "Versus.h"
#include "Input_macro.h"
#include "Utilities/SafeDelete.h"
#include "Rand.h"
#include "LoadHitData.h"

#include "resource.h"

#define THROW_DAMAGE 0.3f
#define STAGE_SIZE 7.5f
#define MIN_DIST 0.7f

#define DEFENSESIZE 13
#define ATTACKSIZE 4

typedef struct {
	LPCTSTR g_vmdname;
	bool loop_flag;
}VMDDATA;
VMDDATA g_VmdData[] = {
	{ "asset/motion/down.vmd", false },
	{ "asset/motion/stand_1.vmd", true },
	{ "asset/motion/walk_1.vmd", true },
	{ "asset/motion/pan.vmd", false },
	{ "asset/motion/kik.vmd", false },
	{ "asset/motion/sp_pan.vmd", false },
	{ "asset/motion/sp_kik.vmd", false },
	{ "asset/motion/www.vmd", false },
};

CPU_ACTION CPUActionNear[] = {
	{ 1, CMD5PG, 0 },
	{ 1, CMD6236K, 0 },
	{ 1, CMD5PP, 0 },
	{ 1, CMD3K, 0 },
	{ 1, CMD2K, 0 },
	{ 1, CMD2P, 0 },
	{ 3, CMD5P, 0 },
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
//void set_status();
//void get_mode();
//void set_mode();
//
///* 関数テーブルの型 */
//typedef void(*FUNCTBL)(void);
//
///* 関数テーブル */
//FUNCTBL func_tbl[] = {
//	set_status,
//	get_mode,
//	set_mode
//};

HitA* Player::HitDefense;
HitA* Player::HitAttack;

SmartPointer<CSkinMeshEffect> g_Effect(new CSkinMeshEffect());
CRand Rand;

BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
Player::Player(LPDIRECT3DDEVICE9 lpd3ddev, int CPUMode, int id) : m_lpd3ddevice(lpd3ddev), m_CPU(CPUMode), m_id(id)
{
	if (id == 0){
		m_Angle = D3DXVECTOR3(0, -90, 0);
		m_Translation = D3DXVECTOR3(-1.8f, 0, 0.);
	}
	else{
		m_Angle = D3DXVECTOR3(0, 90, 0);
		m_Translation = D3DXVECTOR3(1.8f, 0, 0.);
	}
	// PMXモデル生成
	bool sts;
	m_model = new PmxSkinMesh();
	m_model->SetDevice(lpd3ddev);
	sts = m_model->LoadModel("asset/model/sakuya/sakuya(kari).pmx", lpd3ddev);
	if (!sts){
		MessageBox(NULL, "ERROR!!", "load model", MB_OK);
		return;
	}
	// モーション生成
	for (int i = 0; i < sizeof(g_VmdData) / sizeof(VMDDATA); i++){
		m_motion[i] = new VmdMotionController();
		m_motion[i]->LoadVmdFile(g_VmdData[i].g_vmdname, m_model->GetBoneAddress(), m_model->GetIkAddress(), g_VmdData[i].loop_flag);
	}

	// エフェクトファイル生成
	g_Effect->LoadEffect("effect/skinmesh.fx");

	HitDefense = new HitA[DEFENSESIZE]();
	HitAttack = new HitA[ATTACKSIZE]();

	LoadData();

	// ボーン名からIDを取得と初期化
	for (int i = 0; i < DEFENSESIZE; i++){
		HitDefense[i].SetBoneIndex(m_model->GetBoneId(HitDefense[i].GetBoneName()));
		HitDefense[i].Setpos(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		D3DXMatrixIdentity(&HitDefense[i].Getworld());
	}
	for (int i = 0; i < ATTACKSIZE; i++){
		HitAttack[i].SetBoneIndex(m_model->GetBoneId(HitAttack[i].GetBoneName()));
		HitAttack[i].Setpos(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		D3DXMatrixIdentity(&HitAttack[i].Getworld());
	}
	for (int i = 0; i < sizeof(HitAttackR) / sizeof(HIT); i++){
		HitAttackR[i].BoneIndex = m_model->GetBoneId(HitAttackR[i].BoneName);
		HitAttackR[i].pos = { 0.0f, 0.0f, 0.0f };
		D3DXMatrixIdentity(&HitAttackR[i].world);
	}
	// ゲージ生成
	m_gauge = new CGauge(id);

	m_rigidity = 0;
	CPU = false;
	CPUCommand = NULL;
	CPUCommandTime = 0;
	CPURepeat = 0;
	Throw = DownAttack = false;
	WaitTime = 0;
	HalfBody = false;
	NextAnimIndex = -1;
	AnimIndex = oldAnimIndex = 1;
	m_moveflag = false;
	m_Reverse = (id == 1);
	Rand.Init(1);
	m_id ? printf("P2を読み込みました\n") : printf("P1を読み込みました\n");
}

////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
Player::~Player()
{
	// ゲージ破棄
	SAFE_DELETE(m_gauge);
	SAFE_DELETE_ARRAY(HitAttack);
	SAFE_DELETE_ARRAY(HitDefense);
	// モーション破棄
	for (int i = 0; i < sizeof(g_VmdData) / sizeof(VMDDATA); i++){
		SAFE_DELETE(m_motion[i]);
	}
	// モデル破棄
	SAFE_DELETE(m_model);
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
	m_model->SetMotion(m_motion[AnimIndex]);
	m_model->SetTech(g_Effect);
	MakeTotalMatrix();
	//入力を１つ移動する
	for (int i = 0; i < INPUT_COUNT; i++){
		m_nInput[i] = INPUT_NOT5;
	}
}

////////////////////////////////////////////////////////////////
// 入力処理
////////////////////////////////////////////////////////////////
void Player::Input()
{
	D3DXVECTOR3 v;
	D3DXVECTOR3 a;
	m_Speed = 0;
	InputJudge();
	if (m_rigidity == 0){
		CommandJudge();
	}
	if (INPUT.CheckKeyBuffer(DIK_SPACE)){
		v = D3DXVECTOR3(0, 0.2f,0);
		a = D3DXVECTOR3(0, -0.01f, 0);
	}
	// 吹き飛び
	if (INPUT.CheckKeyBuffer(DIK_SPACE)){
		v += a;
		m_Translation += v;
	}
	else {
		m_Translation += D3DXVECTOR3(0, -0.2f, 0);
		if (m_Translation.y < 0.0f){
			m_Translation.y = 0;
		}
	}

	//入力がなければ
	m_moveflag = CheckInput(CMD5) && m_rigidity == 0 ? false : true;
}

////////////////////////////////////////////////////////////////
// 更新処理
////////////////////////////////////////////////////////////////
void Player::Update()
{
	// 動作の切り替え
	int index = AnimIndex;
	VmdMotionController* old = m_motion[oldAnimIndex];
	Throw = DownAttack = false;
	// ダメージとガード
	if (NextAnimIndex != -1) {
		//CheckHalfBody();
		//old->ResetTime();
		//AnimIndex = NextAnimIndex;
		//NextAnimIndex = -1;
	}

	if (m_rigidity != 0){
		m_Speed = 0.0f;
	}
	if (m_moveflag){
		m_Translation.x += m_Speed;
		//範囲外に行かないようにする
		if (m_Translation.x < -STAGE_SIZE){
			m_Translation.x += -STAGE_SIZE - m_Translation.x;
		}
		if (m_Translation.x > STAGE_SIZE){
			m_Translation.x += STAGE_SIZE - m_Translation.x;
		}
		m_rigidity--;
	}
	else{
		m_Speed = 0;
		AnimIndex = 1;
	}
	if (m_rigidity < 0){
		m_rigidity = 0;
		for (int j = 0; j < sizeof(Attack) / sizeof(ATTACK); j++){
			AttackValid[j] = true;
		}
	}
	VmdMotionController* now = m_motion[AnimIndex];
	D3DXVECTOR3 pos;
	pos = {
		m_model->GetBoneAddress()->operator[](0).GetModelLocalPosition().x,
		m_model->GetBoneAddress()->operator[](0).GetModelLocalPosition().y,
		m_model->GetBoneAddress()->operator[](0).GetModelLocalPosition().z,
	};
	//アニメーションが前回と違うと更新
	if (old != now){
		m_motion[oldAnimIndex]->ResetTime();
		// 位置更新
		D3DXMATRIX& m = m_model->GetBoneAddress()->operator[](0).GetCombinedMatrix();
		m_Translation = D3DXVECTOR3(m._41, m._42, m._43);
		m_model->SetMotion(m_motion[AnimIndex]);
		oldAnimIndex = AnimIndex;
	}
	// タイムが超えた時の処理
	if (m_motion[AnimIndex]->GetOverFlag()){
		// 位置更新
		D3DXMATRIX& m = m_model->GetBoneAddress()->operator[](0).GetCombinedMatrix();
		m_Translation = D3DXVECTOR3(m._41, m._42, m._43);
		m_motion[AnimIndex]->SetOverFlag(false);
	}
	//アニメーションの時間を進める
	m_motion[AnimIndex]->AdvanceTime();
	//ボーンを更新する
	m_motion[AnimIndex]->UpdateBoneMatrix();
	m_model->GetBoneAddress()->operator[](0).SetCombinedMatrix(m_MatWork, pos);
	MakeTotalMatrix();
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
			else{
				if (AnimIndex == ANIM_DOWN0){
					if (Rand.Real1() < 0.1f){
						CPUCommand = Rand.Real1() < 0.5f ? CMD5K : CMD2K;
					}
					else {
						CPUCommand = Rand.Real1() < 0.5f ? CMD5 : CMD2;
					}
					CPURepeat = 0;
				}
				else {
					D3DXVECTOR3 v = Enemy->GetTranslation() - m_Translation;
					float dist = D3DXVec3Length(&v);
					CPU_ACTION* action;
					int i, n, sum;
					if (dist <= 1.2f*1.5f){
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
		AnimIndex = 7;
	}
	//CMD2
	else if (CheckInput(CMD2)){
		AnimIndex = 0;
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
		AnimIndex = 2;
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
		AnimIndex = 2;
	}
	//CMDP
	if (CheckInput(CMD5P)){
		AnimIndex = 3;
		m_rigidity = Attack[0].ToTime;
	}
	if(CheckInput(CMD5K)){
		AnimIndex = 4;
		m_rigidity = Attack[1].ToTime;
	}

	//CMD236P
	if (CheckInput(CMD236P)){
		AnimIndex = 5;
		m_rigidity = Attack[2].ToTime;
	}
	if (CheckInput(CMD6236K)){
		AnimIndex = 6;
		m_rigidity = Attack[3].ToTime;
	}
}

void Player::LoadData()
{
	CLoadHitData* LoadDefense = new CLoadHitData("defense.txt");
	for (int i = 0; i < DEFENSESIZE; i++){
		HitDefense[i].SetBoneName(LoadDefense->LoadName());
		HitDefense[i].SetRadius(LoadDefense->LoadRadius());
		LoadDefense->Reset();
	}
	CLoadHitData* LoadAttack = new CLoadHitData("attack.txt");
	for (int i = 0; i < ATTACKSIZE; i++){
		HitAttack[i].SetBoneName(LoadAttack->LoadName());
		HitAttack[i].SetRadius(LoadAttack->LoadRadius());
		LoadAttack->Reset();
	}
	SAFE_DELETE(LoadAttack);
	SAFE_DELETE(LoadDefense);
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
void Player::SetAngle(float x, float y, float z)
{
	m_Angle.x = x;
	m_Angle.y = y;
	m_Angle.z = z;
}

////////////////////////////////////////////////////////////////
// 平行移動量をセット（前回描画して以降の移動量）
////////////////////////////////////////////////////////////////
void Player::SetTranslation(float x, float y, float z)
{
	m_Translation.x += x;
	m_Translation.y += y;
	m_Translation.z += z;
}

void Player::GetDefenseBuff(char* buff, int i)
{
	sprintf(buff, "%.2f", HitDefense[i].GetRadius());
}

void Player::GetAttackBuff(char* buff, int i)
{
	sprintf(buff, "%.2f", HitAttack[i].GetRadius());
}



//********************************************************************************************
// Versus
//********************************************************************************************

#define GETMODEL(i,j)	m_player[i]->GetModel()->GetBoneAddress()->operator[](j)

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
Versus::Versus()
{
	// プレイヤー生成
	for (int i = 0; i < 2; i++){
		m_player[i] = new Player(m_lpd3ddevice, i, i);
	}
	for (int i = 0; i < 2; i++){
		m_player[i]->Enemy = m_player[1 - i];
	}
	// 当たり判定用球生成
	m_HitBall = new CHitBox("asset/model/Ball.x");
	// デバッグフォント生成
	m_font = new CDebugFont(35);
	m_CPUMode = 0;
	m_HitMode = 1;
	printf("Versusを生成しました\n");
}

////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
Versus::~Versus()
{
	// デバッグフォント破棄
	SAFE_DELETE(m_font);
	// 当たり判定用球破棄
	SAFE_DELETE(m_HitBall);
	// プレイヤー破棄
	for (int i = 0; i < 2; i++){
		SAFE_DELETE(m_player[i]);
	}
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
	if (INPUT.CheckKeyBufferTrigger(DIK_F3)){
		// ダイアログボックス生成
		DialogBox(m_hinstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc);
	}
	if (INPUT.CheckKeyBufferTrigger(DIK_F6)){
		m_GameEndFlag = true;
	}
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
	D3DXMATRIX& m0 = GETMODEL(0, 0).GetCombinedMatrix();
	D3DXMATRIX& m1 = GETMODEL(1, 0).GetCombinedMatrix();
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
		float time = (float)player->GetAnimation();
		for (int j = 0, jn = sizeof(Attack) / sizeof(ATTACK); j < jn; j++) {
			ATTACK* attack = &Attack[j];
			if (player->AttackValid[j] && attack->AnimIndex == player->AnimIndex &&
				attack->FromTime <= time && time <= attack->ToTime && CheckHit(i, attack->HitIndex)){
				player->AttackValid[j] = false;
				enemy->NextAnimIndex = defense->NextAnimIndex[attack->Position];
				enemy->WaitTime = attack->WaitTime * 100;
				if (Defense[enemy->NextAnimIndex].Attribute&DA_DAMAGE){
					m_player[1 - i]->GetGauge()->Damage(attack->Damage);
				}
				if (defense->Attribute&DA_COUNTER){
					enemy->WaitTime *= (int)1.5f;
					if (Defense[enemy->NextAnimIndex].Attribute&DA_DAMAGE){
						m_player[1 - i]->GetGauge()->Damage(attack->Damage*0.5f);
					}
					if (player->AnimIndex == ANIM_3236P || player->AnimIndex == ANIM_5K){
						enemy->NextAnimIndex = defense->NextAnimIndex[AP_BLOW];
					}
				}
				break;
			}
		}
		if (player->Throw && dist <= MIN_DIST*1.1f && (defense->Attribute&DA_THROWABLE) && (!enemy->Throw || (defense->Attribute&DA_THROWABLE) == 0)){
			player->NextAnimIndex = 0;
			enemy->NextAnimIndex = 0;
			m_player[1 - i]->GetGauge()->Damage(THROW_DAMAGE);
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
	D3DXMATRIX mat;
	// 当たり判定更新
	if (m_HitMode>0){
		for (int i = 0; i<2; i++){
			//Hit* hit;
			HitA* hit;
			int n;
			if ((m_HitMode + i) % 2 == 0){
				hit = m_player[i]->GetDefense();
				n = DEFENSESIZE;
			}
			else{
				hit = m_player[i]->GetAttack();
				n = ATTACKSIZE;
			}
			for (int j = 0; j<n; j++){
				hit[j].Setpos(D3DXVECTOR3(GETMODEL(i, hit[j].GetBoneIndex()).GetModelLocalPosition().x,
					GETMODEL(i, hit[j].GetBoneIndex()).GetModelLocalPosition().y,
					GETMODEL(i, hit[j].GetBoneIndex()).GetModelLocalPosition().z));
				m_HitBall->Update(mat, m_player[i]->GetMatrixWorld(), hit[j].Getpos(), hit[j].GetRadius());
				hit[j].Setworld(mat);
			}
		}
	}

	// ゲージ
	for (int i = 0; i<2; i++) {
		m_player[i]->GetGauge()->Move();
		if (m_player[i]->GetGauge()->Value <= 0 && m_player[i]->GetGauge()->PrevValue <= 0) {
			/*m_player[1 - i]->GetGauge()->WinCount++;
			m_player[i]->GetGauge()->Reset();*/
			if (INPUT.CheckKeyBufferTrigger(DIK_SPACE)){
				m_GameEndFlag = true;
			}
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

	// 当たり判定描画
	if (m_HitMode>0){
		for (int i = 0; i<2; i++){
			//Hit* hit;
			HitA* hit;
			int n;
			if ((m_HitMode + i) % 2 == 0){
				hit = m_player[i]->GetDefense();
				n = DEFENSESIZE;
			}
			else{
				hit = m_player[i]->GetAttack();
				n = ATTACKSIZE;
			}
			for (int j = 0; j<n; j++){
				if (hit == m_player[i]->GetAttack()){
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
				m_HitBall->Draw(hit[j].Getworld());
			}
		}
	}

	RECT rc;
	for (int i = 0; i < 2; i++){
		if (m_player[i]->GetGauge()->Value <= 0 && m_player[i]->GetGauge()->PrevValue <= 0) {
			SetRect(&rc, 350, 300, 0, 0);
			m_font->Draw("PUSH SPACE!", -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));
		}
	}
}


////////////////////////////////////////////////////////////////
// 当たり判定
////////////////////////////////////////////////////////////////
bool Versus::CheckHit(int attack_player_id, int attack_hit_id)
{
	HitA* hit_attack = m_player[attack_player_id]->GetAttack();
	D3DXVECTOR3 pos;
	pos.x = GETMODEL(attack_player_id, hit_attack[attack_hit_id].GetBoneIndex()).GetModelLocalPosition().x;
	pos.y = GETMODEL(attack_player_id, hit_attack[attack_hit_id].GetBoneIndex()).GetModelLocalPosition().y;
	pos.z = GETMODEL(attack_player_id, hit_attack[attack_hit_id].GetBoneIndex()).GetModelLocalPosition().z;
	D3DXVECTOR3 m_wpos;
	D3DXVec3TransformCoord(&m_wpos, &pos, &m_player[attack_player_id]->GetMatrixWorld());
	D3DXMATRIX& am = m_player[attack_player_id]->GetMatrixWorld();
	am._41 = m_wpos.x;
	am._42 = m_wpos.y;
	am._43 = m_wpos.z;
	D3DXVECTOR3 apos(am._41, am._42, am._43);
	HitA* hit_defense = m_player[1 - attack_player_id]->GetDefense();
	for (int i = 0, n = ATTACKSIZE; i < n; i++){
		pos.x = GETMODEL(1 - attack_player_id, hit_defense[i].GetBoneIndex()).GetModelLocalPosition().x;
		pos.y = GETMODEL(1 - attack_player_id, hit_defense[i].GetBoneIndex()).GetModelLocalPosition().y;
		pos.z = GETMODEL(1 - attack_player_id, hit_defense[i].GetBoneIndex()).GetModelLocalPosition().z;
		D3DXVec3TransformCoord(&m_wpos, &pos, &m_player[1 - attack_player_id]->GetMatrixWorld());
		D3DXMATRIX& dm = m_player[1 - attack_player_id]->GetMatrixWorld();
		dm._41 = m_wpos.x;
		dm._42 = m_wpos.y;
		dm._43 = m_wpos.z;
		D3DXVECTOR3 dpos(dm._41, dm._42, dm._43);
		D3DXVECTOR3 dist = apos - dpos;
		if (D3DXVec3Length(&dist) <= hit_attack[attack_hit_id].GetRadius() + hit_defense[i].GetRadius()){
			return true;
		}
	}
	return false;
}


/*-------------------------------------

メッセージ処理関数（ウインドウ関数）
この関数は、ＯＳから呼び出されてメッセージキューから
メッセージの引渡しを受ける

--------------------------------------*/
BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char buff[256];
	char buf[256];
	char aaa[256];
	int ccnt;
	switch (uMsg){
	case WM_INITDIALOG:
		for (int i = 0; i < ATTACKSIZE; i++){
			Player::GetAttackBuff(buff, i);
			SetDlgItemText(hwndDlg, IDC_EDIT1 + i, buff);
		}
		for (int i = 0; i < DEFENSESIZE; i++){
			Player::GetDefenseBuff(buff, i);
			SetDlgItemText(hwndDlg, IDC_EDIT7 + i, buff);
		}
		return true;

	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case IDOK:
			CReadHitData* ReadAttack = new CReadHitData("attack.txt");
			for (int i = 0; i < ATTACKSIZE; i++){
				ccnt = GetDlgItemText(hwndDlg, IDC_EDIT1 + i, buf, sizeof(buf));
				GetDlgItemText(hwndDlg, IDC_STATIC1 + i, buff, sizeof(buff));
				if (ccnt == 0){
					sprintf(aaa, "%s,0.0f\n", buff);
				}
				else{
					sprintf(aaa, "%s,%sf\n", buff, buf);
				}
				ReadAttack->ReadData(aaa);
			}
			SAFE_DELETE(ReadAttack);

			CReadHitData* ReadDefense = new CReadHitData("defense.txt");
			for (int i = 0; i < DEFENSESIZE; i++){
				ccnt = GetDlgItemText(hwndDlg, IDC_EDIT7 + i, buf, sizeof(buf));
				GetDlgItemText(hwndDlg, IDC_STATIC5 + i, buff, sizeof(buff));
				if (ccnt == 0){
					sprintf(aaa, "%s,0.0f\n", buff);
				}
				else{
					sprintf(aaa, "%s,%sf\n", buff, buf);
				}
				ReadDefense->ReadData(aaa);
			}
			SAFE_DELETE(ReadDefense);
			Player::LoadData();
			EndDialog(hwndDlg, IDEXIT);
			return true;

		}
		break;
		
	case WM_CLOSE:
		EndDialog(hwndDlg, IDEXIT);
		return true;
		break;
	}
	
	return false;
}
