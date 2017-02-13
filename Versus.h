#pragma once
#include "Utilities/GameObj.h"
#include "Chr/CCharacterMng.h"
#include "SkinMesh/SkinMesh.h"
#include "Camera/CamaraTPS.h"
#include "Utilities/Math.h"
#include "Utilities/Input.h"
#include "Input_macro.h"
#include "HitBox.h"
#include "UI/Gauge.h"
#include "Utilities/CDebugFont.h"

// �����蔻��
struct HIT {
	string BoneName;
	float Radius;
	int BoneIndex;
	D3DXMATRIX world;
	D3DXVECTOR3 pos;
};
//static HIT HitDefense[] = {
//	{ "��", 0.01f },
//	{ "�㔼�g", 0.2f },
//	{ "��", 0.2f },
//	{ "����", 0.15f },
//	{ "�E��", 0.15f },
//	{ "���Ђ�", 0.15f },
//	{ "�E�Ђ�", 0.15f },
//	{ "����", 0.15f },
//	{ "�E��", 0.15f },
//	{ "���Ђ�", 0.15f },
//	{ "�E�Ђ�", 0.15f },
//	{ "������", 0.15f },
//	{ "�E����", 0.15f },
//};
//static HIT HitAttack[] = {
//	{ "���_�~�[", 0.1f },
//	{ "�E�_�~�[", 0.2f },
//	{ "������", 0.2f },
//	{ "�E����", 0.1f },
//};
static HIT HitAttackR[] = {
	{ "�E�_�~�[", 0.1f },
	{ "���_�~�[", 0.1f },
	{ "�E����", 0.1f },
	{ "������", 0.1f },
};
enum {
	HIT_P_L,
	HIT_P_R,
	HIT_K_L,
	HIT_K_R,
	HIT_D8P_L,
	HIT_D8P_R,
};

// �U���Ɩh��
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
	{ 3, AP_MIDDLE, HIT_P_L, 1, 20, 5, 0.1f },
	{ 4, AP_MIDDLE, HIT_K_R, 1, 20, 5, 0.1f },
	{ 5, AP_MIDDLE, HIT_P_R, 40, 100, 20, 0.2f },
	{ 6, AP_MIDDLE, HIT_K_R, 25, 35, 9, 0.1f },
};

// �h��̌���
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
static DEFENSE Defense[36] = {
	// �ړ�
	{ { -1, ANIM_DAMAGE2MIDDLE0, ANIM_DAMAGE2LOW0, -1, -1 }, 0 },
	{ { ANIM_DAMAGE5HIGH0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5LOW0, -1, -1 }, DA_THROWABLE },
	{ { ANIM_DAMAGE5HIGH0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5LOW0, -1, -1 }, DA_THROWABLE },
	{ { ANIM_DAMAGE5HIGH0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5LOW0, -1, -1 }, DA_THROWABLE },
	{ { ANIM_DAMAGE5HIGH0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5LOW0, -1, -1 }, DA_THROWABLE },
	{ { ANIM_DAMAGE5HIGH0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5LOW0, -1, -1 }, DA_THROWABLE },
	{ { -1, ANIM_DAMAGE2MIDDLE0, ANIM_DAMAGE2LOW0, -1, -1 }, 0 },
	{ { -1, ANIM_DAMAGE2MIDDLE0, ANIM_DAMAGE2LOW0, -1, -1 }, 0 },
	{ { ANIM_DAMAGE5HIGH0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5LOW0, -1, -1 }, DA_THROWABLE },

	// �K�[�h�A�U��
	{ { -1, ANIM_DAMAGE2MIDDLE0, ANIM_GUARD2, -1, -1 }, 0 },
	{ { -1, ANIM_DAMAGE2MIDDLE0, ANIM_DAMAGE2LOW0, -1, ANIM_BLOW20 }, DA_COUNTER },
	{ { -1, ANIM_DAMAGE2MIDDLE0, ANIM_DAMAGE2LOW0, -1, ANIM_BLOW20 }, DA_COUNTER },
	{ { ANIM_GUARD5, ANIM_GUARD5, ANIM_DAMAGE5LOW0, -1, -1 }, DA_THROWABLE },
	{ { ANIM_DAMAGE5HIGH0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5LOW0, -1, ANIM_BLOW50 }, DA_THROWABLE | DA_COUNTER },
	{ { ANIM_DAMAGE5HIGH0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5LOW0, -1, ANIM_BLOW50 }, DA_THROWABLE | DA_COUNTER },
	{ { ANIM_DAMAGE5HIGH0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5LOW0, -1, ANIM_BLOW50 }, DA_THROWABLE | DA_COUNTER },
	{ { ANIM_DAMAGE5HIGH0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5LOW0, -1, ANIM_BLOW50 }, DA_THROWABLE | DA_COUNTER },
	{ { ANIM_DAMAGE5HIGH0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5LOW0, -1, ANIM_BLOW50 }, DA_THROWABLE | DA_COUNTER },

	// �_���[�W
	{ { -1, ANIM_DAMAGE2MIDDLE0, ANIM_DAMAGE2LOW0, -1, -1 }, DA_DAMAGE },
	{ { -1, ANIM_DAMAGE2MIDDLE0, ANIM_DAMAGE2LOW0, -1, -1 }, DA_DAMAGE },
	{ { ANIM_DAMAGE5HIGH0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5LOW0, -1, -1 }, DA_THROWABLE | DA_DAMAGE },
	{ { ANIM_DAMAGE5HIGH0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5LOW0, -1, -1 }, DA_THROWABLE | DA_DAMAGE },
	{ { ANIM_DAMAGE5HIGH0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5LOW0, -1, -1 }, DA_THROWABLE | DA_DAMAGE },

	// ������сA�_�E���A�_�E���U��
	{ { ANIM_BLOW20, ANIM_BLOW20, ANIM_BLOW20, -1, -1 }, DA_DAMAGE },
	{ { ANIM_BLOW50, ANIM_BLOW50, ANIM_BLOW50, -1, -1 }, DA_DAMAGE },
	{ { -1, -1, -1, ANIM_DAMAGE_DOWN0, -1 }, 0 },
	{ { -1, -1, -1, -1, -1 }, 0 },
	{ { -1, -1, -1, -1, -1 }, DA_DAMAGE },

	// �K�[�h����
	{ { -1, ANIM_DAMAGE2MIDDLE0, ANIM_GUARD2, -1, -1 }, DA_GUARD },
	{ { ANIM_GUARD5, ANIM_GUARD5, ANIM_DAMAGE5LOW0, -1, -1 }, DA_THROWABLE | DA_GUARD },

	// ����
	{ { ANIM_DAMAGE5HIGH0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5LOW0, -1, -1 }, 0 },
	{ { -1, -1, -1, -1, -1 }, 0 },

	// �N��������A�N��������U��
	{ { -1, -1, -1, -1, -1 }, 0 },
	{ { -1, ANIM_DAMAGE2MIDDLE0, ANIM_DAMAGE2LOW0, -1, ANIM_BLOW20 }, DA_COUNTER },
	{ { -1, -1, -1, -1, -1 }, 0 },
	{ { ANIM_DAMAGE5HIGH0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5LOW0, -1, ANIM_BLOW50 }, DA_COUNTER },
};

// CPU
struct CPU_ACTION {
	float Rate;	//�s��������
	const int* Command;//�R�}���h
	int Repeat;//�ő�J��Ԃ���
};


#define VERSUS_PRIORITY 50000
#define INPUT_COUNT 60

#include "Hit.h"

class Player : CCharacterMng
{
private:
	PmxSkinMesh*			m_model;
	VmdMotionController*	m_motion[10];
	D3DXMATRIX CenterMatrix;
	LPDIRECT3DDEVICE9 m_lpd3ddevice;
	CMath*					m_math;

	/*Hit *HitDefense;
	Hit *HitAttack;*/
	static HitA *HitDefense;
	static HitA *HitAttack;
	int m_id;
	int m_CPU;
	bool m_moveflag;
	int m_rigidity;
	int						m_nInput[INPUT_COUNT];
	CGauge* m_gauge;
public:
	Player(LPDIRECT3DDEVICE9 lpd3ddev, int CPUMode, int id);
	~Player();
	void Init();
	void Input();
	void Update();
	void Render(D3DXMATRIX view, D3DXMATRIX projection);
	bool CheckInput(const int *command);
	void InputJudge();
	void CommandJudge();
	void MakeTotalMatrix();
	void SetAngle(float x, float y, float z);
	void SetTranslation(float x, float y, float z);

	static void LoadData();
	static void GetDefenseBuff(char* buff, int i);
	static void GetAttackBuff(char* buff, int i);

	D3DXMATRIX GetMatrixTotal(){ return m_MatTotal; }
	D3DXMATRIX GetMatrixWorld(){ return m_MatWork; }
	PmxSkinMesh* GetModel(){ return m_model; }
	int GetAnimation(){ return m_motion[AnimIndex + (HalfBody ? 1 : 0)]->GetTime(); }
	CGauge* GetGauge(){ return m_gauge; }
	D3DXVECTOR3 GetTranslation(){ return m_Translation; }
	/*Hit* GetDefense(){ return HitDefense; }
	Hit* GetAttack(){ return HitAttack; }*/

	HitA* GetDefense(){ return HitDefense; }
	HitA* GetAttack(){ return HitAttack; }

	bool CPU;
	const int* CPUCommand;
	int CPUCommandTime;
	int CPURepeat;

	bool m_Reverse;
	bool HalfBody;
	int AnimIndex;
	bool Throw;
	bool DownAttack;
	int NextAnimIndex;
	int oldAnimIndex;
	Player* Enemy;
	int WaitTime;
	bool AttackValid[sizeof(Attack) / sizeof(ATTACK)];
};


class Versus : public CGameObj
{
private:
	Player* m_player[2];
	CCamaraTPS*	camera;
	D3DXVECTOR3 center;
	D3DXVECTOR3 unit;
	CDebugFont* m_font;
	float dist;
	int m_HitMode;
	int m_CPUMode;
	CHitBox* m_HitBall;
	int Dsize;
public:
	Versus();
	~Versus();
	bool CheckHit(int attack_player_id, int attack_hit_id);
	D3DXVECTOR3 GetCenter(){ return center; }
	D3DXVECTOR3 GetUnit(){ return unit; }
	float GetDist(){ return dist; }
protected:
	virtual void Init();
	virtual void Input();
	virtual void Update();
	virtual void Render();
};
