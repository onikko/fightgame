#pragma once
#include "../Utilities/GameObj.h"
#include "../Utilities/CDirect3DXFile.h"

#define HIT_ATTACK_POS (4)
#define HIT_DEFENSE_POS (13)
// アニメーションID
//enum ANIM_ID {
//
//	// 移動
//	ANIM_2, ANIM_5,
//	ANIM_4, ANIM_6,
//
//	//ジャンプ
//	ANIM_JUMP,
//
//	// ガード、攻撃
//	ANIM_2G, ANIM_2K, ANIM_2P,
//	ANIM_5G, ANIM_5K, ANIM_5P,
//	ANIM_3K, ANIM_5PP, ANIM_3236P,
//
//	// ダメージ
//	ANIM_DAMAGE2LOW0, ANIM_DAMAGE2MIDDLE0,
//	ANIM_DAMAGE5LOW0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5HIGH0,
//
//	// ガード成功
//	ANIM_GUARD2, ANIM_GUARD5,
//
//	// 投げ
//	ANIM_5PG, ANIM_THROWN5PG,
//
//	ANIM_MAX
//};

typedef struct{
	float x, y, z;
	D3DXMATRIX world;
	float radius;
	int id;
	bool flag;
}HIT;

class CCharacterMng : public CGameObj
{
public:
	virtual D3DXMATRIX GetMatrixTotal() = 0;
	virtual void SetAngle(float x, float y, float z){}
	virtual void SetTranslation(float x, float y, float z){}
protected:
	D3DXMATRIX			m_MatTotal;
	D3DXMATRIX			m_MatWork;				// ワーク用の行列
	D3DXVECTOR3			m_Angle;				// ローカル座標の各軸に対する角度（前回描画して以降の回転量）
	D3DXVECTOR3			m_Translation;			// ローカル座標上の平行移動量（前回描画して以降の移動量）
	float				m_Speed;
};

