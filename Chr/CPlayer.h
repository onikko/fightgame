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
#include "../UI/Gauge.h"
#include "../DebugMgr.h"

#define PLAYER_MOTION_MAX 10
#define INPUT_COUNT 60
#define PLAYER_PRIORITY 50000
#define BULLET_PRIORITY 55000

class CPlayer : public CCharacterMng
{	
private:
	CCamaraTPS* camera;
	sp<PmxSkinMesh>			m_model;
	sp<VmdMotionController> m_motion[PLAYER_MOTION_MAX];
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
	HIT HitDefense[HIT_DEFENSE_POS];
	HIT HitAttack[HIT_ATTACK_POS];
	int id;
	float damege;
public:
	CPlayer(D3DXVECTOR3 angle, D3DXVECTOR3 translation, bool moveflag);
	~CPlayer();
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
	HIT* GetDefenseData(){ return HitDefense; };
	HIT* GetAttack(){ return HitAttack; };
	D3DXVECTOR3 GetTranslation(){ return m_Translation; }
	CGauge* GetGaugeData(){ return gauge; }
	int GetID(){ return id; }
	float GetDamege(){ return damege; }
protected:
	virtual void Init();
	virtual void Input();
	virtual void Update();
	virtual void Render();
};

class CPlayerBullet : public CCharacterMng
{
private:
	D3DMATERIAL9 m_material;//半透明マテリアル
	D3DXVECTOR3 m_wpos;		//ワールド空間上の位置座標
	LPD3DXMESH m_spheremesh;//球メッシュ
	CMath* m_math;
public:
	CPlayerBullet(D3DXMATRIX mat, float x, float y, float z);
	~CPlayerBullet();
	D3DXMATRIX GetMatrixTotal(){ return m_MatTotal; }
	void TraneparentMaterial(float alpha);
protected:
	virtual void Init();
	virtual void Update();
	virtual void Render();
};