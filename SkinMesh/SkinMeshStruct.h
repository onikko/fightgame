#pragma once
///// �X�L�����b�V���A�j���[�V�����p�\���� /////

///// ���̒�` �E�E�E �{�w�b�_���ł͈ȉ��̂悤�Ƀ{�[���̍��W�n�̖��̂��`����	/////
///// ���[���h���W�n�F ��΍��W�n												/////
///// ���f�����[�J�����W�n�F���[���h���W�ϊ����܂܂Ȃ�3D���f���̃��[�J�����W�n	/////
///// �{�[�����[�J�����W�n�F���g�̐e�{�[������ɂ������[�J�����W�n			/////

#include <d3d9.h>
#include <d3dx9.h>

// �u�����h�d�ݕt���b�V���̒��_�f�[�^
struct BlendVertex {
	D3DXVECTOR3 position;
	float weight[3];
	unsigned short matrixIndex[4];
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texture;
};

// �{�[��
struct Bone {
	int id;					// �{�[��ID�i�ʂ��ԍ��j
	string name;			// �{�[����
	unsigned char type;		// �{�[���^�C�v (MMD�̏ꍇ 0:��] 1:��]�ƈړ� 2:IK 3:�s�� 4:IK�e���� 5:��]�e���� 6:IK�ڑ��� 7:��\�� 8:�P�� 9:��]�^�� )
	Bone* parent;			// �e�{�[��
	Bone* firstChild;		// ��1�q�{�[��
	Bone* sibling;			// ���̌Z��{�[��
	D3DXMATRIX initMat;		// �����p���s��(�{�[�����[�J�����W�n)
	D3DXMATRIX boneMat;		// �{�[���s��(�{�[�����[�J�����W�n)
	D3DXMATRIX offsetMat;	// �{�[���I�t�Z�b�g�s��(���f�����[�J�����W�n)
	D3DXMATRIX combinedMat;
	Bone() : type(), id(), parent(), firstChild(), sibling() {	// �R���X�g���N�^
		D3DXMatrixIdentity(&initMat);
		D3DXMatrixIdentity(&offsetMat);
		D3DXMatrixIdentity(&boneMat);
	};
	D3DXMATRIX GetModelLocalBoneMat() {				// ���f�����[�J�����W�n�ł̃{�[���s����擾
		if (parent) return boneMat*parent->GetModelLocalBoneMat();
		else return boneMat;
	}
	D3DXVECTOR3 Bone::GetModelLocalPosition() {		// ���f�����[�J�����W�n�ł̃{�[���ʒu���擾
		D3DXVECTOR3 v(0, 0, 0);
		return *D3DXVec3TransformCoord(&v, &v, &GetModelLocalBoneMat());
	}
	void SetCombinedMatrix(D3DXMATRIX& m, D3DXVECTOR3& pos){
		D3DXVECTOR3 m_wpos;
		D3DXVec3TransformCoord(&m_wpos, &pos, &m);
		combinedMat = m;
		combinedMat._41 = m_wpos.x;
		combinedMat._42 = m_wpos.y;
		combinedMat._43 = m_wpos.z;
	}
	D3DXMATRIX GetCombinedMatrix(){
		return combinedMat;
	}
};

// �L�[�t���[���A�j���[�V�����f�[�^
struct KeyFrame {
	string boneName;		// �{�[����
	unsigned long frameNo;	// �t���[���ԍ�
	D3DXVECTOR3 position;	// �ʒu
	D3DXQUATERNION rotation;// ��]
	D3DXVECTOR2 interpolation_x[2];	// position.x�̃x�W�F�⊮�p�����[�^
	D3DXVECTOR2 interpolation_y[2];	// position.y�̃x�W�F�⊮�p�����[�^
	D3DXVECTOR2 interpolation_z[2];	// position.z�̃x�W�F�⊮�p�����[�^
	D3DXVECTOR2 interpolation_r[2];	// rotation�̃x�W�F�⊮�p�����[�^
	bool operator < (const KeyFrame &k) const {	// �t���[���ԍ��Ń\�[�g���邽�߂̔�r���Z�q
		return frameNo < k.frameNo;
	}
};