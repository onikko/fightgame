#pragma once
#include "EnemyBase.h"
#include "../Utilities/CBoundingSphere.h"
#include "../SkinMesh/SkinMesh.h"
#include "../Factory.h"
#include "../Camera/CamaraTPS.h"
#include "../Utilities/Input.h"
#include "../Utilities/CDebugFont.h"
#include "../Utilities/Math.h"
#include "../UI/UserInterface.h"
#include "../UI/Gauge.h"
#include "../DebugMgr.h"

#define ENEMY_MOTION_MAX 10
#define INPUT_COUNT 60
#define PLAYER_PRIORITY 50000

class CEnemy : public CEnemyBase
{
private:
	CCamaraTPS* camera;
	sp<PmxSkinMesh>			m_model;
	sp<VmdMotionController> m_motion[ENEMY_MOTION_MAX];
	CMath* m_math;
	CGauge* gauge;
	CDebugMgr* m_debug;
	bool Reverse;
	int m_nInput[INPUT_COUNT];
	int m_jumpflag;
	int m_motionflag;
	int m_old_motionflag;
	bool m_moveflag;
	int m_rigidity;
	HIT g_HitDefense[HIT_DEFENSE_POS];
	HIT g_HitAttack[HIT_ATTACK_POS];
public:
	CEnemy(D3DXVECTOR3 angle, D3DXVECTOR3 translation, bool moveflag);
	~CEnemy();
	bool CheckInput(const int *command);
	D3DXMATRIX GetMatrixTotal(){ return m_MatTotal; }
	D3DXMATRIX GetMatrixWorld(){ return m_MatWork; }
	float GetRadius();
	void Shoot();
	void SetAngle(float x, float y, float z);
	void SetTranslation(float x, float y, float z);
	void InputJudge();
	void CommandJudge();
	void LoadData();
	void MakeTotalMatrix();
	HIT* GetDefenseData(){ return g_HitDefense; };
	HIT* GetAttack(){ return g_HitAttack; };
	D3DXVECTOR3 GetTranslation(){ return m_Translation; }
	CGauge* GetGaugeData(){ return gauge; }
protected:
	virtual void Init();
	virtual void Input();
	virtual void Update();
	virtual void Render();
};

