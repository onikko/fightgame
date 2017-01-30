#pragma once
#include "CCharacterMng.h"
#include "../Utilities/CBoundingSphere.h"
#include "../SkinMesh/SkinMesh.h"
#include "../Factory.h"
#include "../Camera/CamaraTPS.h"
#include "../Utilities/Input.h"
#include "../Utilities/CDebugFont.h"
#include "../Utilities/Math.h"
#include "../UI/UserInterface.h"
#include "../DebugMgr.h"
#include "../UI/Gauge.h"

#include "../Versus.h"

#define PLAYER_MOTION_MAX 10
#define INPUT_COUNT 60
#define PLAYER_PRIORITY 50000
#define BULLET_PRIORITY 55000


class CPlayer : public CCharacterMng
{	
private:
	//CDebugMgr*				m_debug;
	PmxSkinMesh*			m_model;
	VmdMotionController*	m_motion[PLAYER_MOTION_MAX];
	CMath*					m_math;
	bool					Reverse;
	int						m_nInput[INPUT_COUNT];
	int						m_jumpflag;
	int						m_animIndex;
	int						m_old_animIndex;
	bool m_moveflag;
	int m_rigidity;
	//HIT HitDefense[HIT_DEFENSE_POS];
	//HIT HitAttack[HIT_ATTACK_POS];
	int id;
	float damege;

	CPlayer* Enemy;
	D3DXMATRIX worldMat;
	D3DXMATRIX worldMatsample;

	LPDIRECT3DDEVICE9 m_lpd3ddevice;
public:
	CPlayer(D3DXVECTOR3 angle, D3DXVECTOR3 translation, bool moveflag, LPDIRECT3DDEVICE9 lpd3ddev);
	~CPlayer();
	bool CheckInput(const int *command);
	D3DXMATRIX GetMatrixTotal(){ return m_MatTotal; }
	D3DXMATRIX GetMatrixWorld(){ return m_MatWork; }
	float GetRadius();
	void SetAngle(float x, float y, float z);
	void SetTranslation(float x, float y, float z);
	void InputJudge();
	void CommandJudge();
	void LoadData();
	void MakeTotalMatrix();
	//HIT* GetDefenseData(){ return HitDefense; };
	//HIT* GetAttack(){ return HitAttack; };
	D3DXVECTOR3 GetTranslation(){ return m_Translation; }
	CGauge* GetGaugeData(){ return gauge; }
	int GetID(){ return id; }
	float GetDamege(){ return damege; }
	void Sample();
	D3DXMATRIX GetSample(){ return worldMat; }
	D3DXMATRIX GetSample1(){ return worldMatsample; }

	void SetEnemyData(CPlayer* data){ Enemy = data; }
	CPlayer* GetEnemyData(){ return Enemy; }
	int GetAnimIndexData(){ return m_animIndex; }
	void Init();
	void Input();
	void Update();
	void Render(D3DXMATRIX view, D3DXMATRIX projection);

	int GetAnimation(){ return m_motion[m_animIndex + (HalfBody ? 1 : 0)]->GetTime(); }
	PmxSkinMesh* GetModel(){ return m_model; }

	CGauge* gauge;
	int NextAnimIndex;
	int WaitTime;
	bool Throw;
	bool HalfBody;
	bool AttackValid[10];
	int AttackSize;
};