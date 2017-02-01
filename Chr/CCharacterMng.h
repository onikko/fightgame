#pragma once
#include "../Utilities/GameObj.h"
#include "../Utilities/CDirect3DXFile.h"

#define HIT_ATTACK_POS (4)
#define HIT_DEFENSE_POS (13)
// �A�j���[�V����ID
enum ANIM_ID {

	// �ړ�
	ANIM_2, ANIM_5,
	ANIM_4, ANIM_6,
	ANIM_44, ANIM_66,
	ANIM_121, ANIM_323,
	ANIM_RUN,

	// �K�[�h�A�U��
	ANIM_2G, ANIM_2K, ANIM_2P,
	ANIM_5G, ANIM_5K, ANIM_5P,
	ANIM_3K, ANIM_5PP, ANIM_3236P,

	// �_���[�W
	ANIM_DAMAGE2LOW0, ANIM_DAMAGE2MIDDLE0,
	ANIM_DAMAGE5LOW0, ANIM_DAMAGE5MIDDLE0, ANIM_DAMAGE5HIGH0,

	// ������сA�_�E���A�_�E���U��
	ANIM_BLOW20, ANIM_BLOW50, ANIM_DOWN0,
	ANIM_D8P, ANIM_DAMAGE_DOWN0,

	// �K�[�h����
	ANIM_GUARD2, ANIM_GUARD5,

	// ����
	ANIM_5PG, ANIM_THROWN5PG,

	// �N��������A�N��������U��
	ANIM_STANDUP2, ANIM_STANDUP2K, ANIM_STANDUP5, ANIM_STANDUP5K,

	ANIM_COUNT
};

//typedef struct{
//	float x, y, z;
//	D3DXMATRIX world;
//	float radius;
//	int id;
//	bool flag;
//}HIT;

class CCharacterMng
{
public:
	virtual D3DXMATRIX GetMatrixTotal() = 0;
	virtual void SetAngle(float x, float y, float z){}
	virtual void SetTranslation(float x, float y, float z){}
protected:
	D3DXMATRIX			m_MatTotal;
	D3DXMATRIX			m_MatWork;				// ���[�N�p�̍s��
	D3DXVECTOR3			m_Angle;				// ���[�J�����W�̊e���ɑ΂���p�x�i�O��`�悵�Ĉȍ~�̉�]�ʁj
	D3DXVECTOR3			m_Translation;			// ���[�J�����W��̕��s�ړ��ʁi�O��`�悵�Ĉȍ~�̈ړ��ʁj
	float				m_Speed;
};

