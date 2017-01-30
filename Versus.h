#pragma once
#include "Utilities/GameObj.h"
#include "Chr/CCharacterMng.h"
#include "SkinMesh/SkinMesh.h"
#include "Camera/CamaraTPS.h"
#include "Utilities/Math.h"
#include "Utilities/Input.h"






// ìñÇΩÇËîªíË
struct HIT {
	char* FrameName;
	float Radius;
	int FrameIndex;
};
static HIT HitDefense[] = {
	{ "F185_ROOT", 0.25f },
	{ "F2_CTRL_C_Back", 0.25f },
	{ "F49_DEST_C_Head", 0.2f },
	{ "F26_CTRL_L_Upperarm", 0.15f },
	{ "F47_CTRL_R_Upperarm", 0.15f },
	{ "F16_CTRL_L_Lowerarm", 0.15f },
	{ "F37_CTRL_R_Lowerarm", 0.15f },
	{ "F27_CTRL_L_Wrist", 0.15f },
	{ "F48_CTRL_R_Wrist", 0.15f },
	{ "F22_CTRL_L_Thigh", 0.2f },
	{ "F43_CTRL_R_Thigh", 0.2f },
	{ "F17_CTRL_L_Shin", 0.2f },
	{ "F38_CTRL_R_Shin", 0.2f },
	{ "F7_CTRL_L_Ankle", 0.2f },
	{ "F28_CTRL_R_Ankle", 0.2f },
	{ "F25_CTRL_L_Toe", 0.1f },
	{ "F46_CTRL_R_Toe", 0.1f },
};
static HIT HitAttack[] = {
	{ "F27_CTRL_L_Wrist", 0.1f },
	{ "F48_CTRL_R_Wrist", 0.1f },
	{ "F7_CTRL_L_Ankle", 0.1f },
	{ "F28_CTRL_R_Ankle", 0.1f },
	{ "F14_CTRL_L_Knee", 0.1f },
	{ "F35_CTRL_R_Knee", 0.1f },
};
static HIT HitAttackR[] = {
	{ "F48_CTRL_R_Wrist", 0.1f },
	{ "F27_CTRL_L_Wrist", 0.1f },
	{ "F28_CTRL_R_Ankle", 0.1f },
	{ "F7_CTRL_L_Ankle", 0.1f },
	{ "F35_CTRL_R_Knee", 0.1f },
	{ "F14_CTRL_L_Knee", 0.1f },
};
enum {
	HIT_P_L,
	HIT_P_R,
	HIT_K_L,
	HIT_K_R,
	HIT_D8P_L,
	HIT_D8P_R,
};

// çUåÇÇ∆ñhå‰
enum ATTACK_POSITION {
	AP_HIGH, AP_MIDDLE, AP_LOW, AP_DOWN, AP_BLOW, AP_COUNT
};
struct ATTACK {
	int AnimIndex;
	int Position;
	int HitIndex;
	int FromTime;
	int ToTime;
	int WaitTime;
	float Damage;
};
static ATTACK Attack[] = {
	{ 0, AP_MIDDLE, HIT_P_L, 11, 18, 20, 0.3f },
};

// ñhå‰ÇÃåãâ 
enum DEFENSE_ATTRIBUTE {
	DA_THROWABLE = 1,
	DA_COUNTER = 2,
	DA_DAMAGE = 4,
	DA_GUARD = 8,
};
struct DEFENSE {
	int NextAnimIndex[AP_COUNT];
	int Attribute;
	int CounterAnimIndex;
};
static DEFENSE Defense[1] = {

	// à⁄ìÆ
	{ { -1, 0, 0, -1, -1 }, 0 }
};



#define VERSUS_PRIORITY 50000
#define INPUT_COUNT 60

class Play : CCharacterMng
{
private:
	PmxSkinMesh*			m_model;
	VmdMotionController*	m_motion[10];
	LPDIRECT3DDEVICE9 m_lpd3ddevice;
	CMath*					m_math;
	int m_id;
	int m_CPU;
	bool m_Reverse;
	int						m_nInput[INPUT_COUNT];
public:
	Play(LPDIRECT3DDEVICE9 lpd3ddev, int CPUMode, int id);
	~Play();
	void MakeTotalMatrix();
	void Init();
	void Input();
	void Update();
	void Render(D3DXMATRIX view, D3DXMATRIX projection);
	bool CheckInput(const int *command);
	void InputJudge();
	void CommandJudge();

	D3DXMATRIX GetMatrixTotal(){ return m_MatTotal; }
	D3DXMATRIX GetMatrixWorld(){ return m_MatWork; }
	PmxSkinMesh* GetModel(){ return m_model; }

	bool HalfBody;
	int AnimIndex;
	bool Throw;
	bool DownAttack;
	int NextAnimIndex;
	Play* Enemy;
	int WaitTime;
	bool AttackValid[sizeof(Attack) / sizeof(ATTACK)];
};


class Versus : public CGameObj
{
private:
	CPlayer* m_player[2];
	Play* m_play[2];
	CCamaraTPS*	camera;
	D3DXVECTOR3 center;
	D3DXVECTOR3 unit;
	int m_CPUMode;
public:
	Versus();
	~Versus();
	bool CheckHit(int attack_player_id, int attack_hit_id);
	D3DXVECTOR3 GetCenter(){ return center; }
	D3DXVECTOR3 Get(){ return unit; }
protected:
	virtual void Init();
	virtual void Input();
	virtual void Update();
	virtual void Render();
};
