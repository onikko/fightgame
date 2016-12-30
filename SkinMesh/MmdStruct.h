#pragma once

#include <vector>

using namespace std;

/// MMD��p�\���̒�`
namespace MmdStruct {
#pragma pack(push,1)	//�A���C�������g������I�t

	/// PMD�\���̒�`
	struct PmdHeader {
		unsigned char magic[3];
		float version;
		unsigned char model_name[20];
		unsigned char comment[256];
	};
	struct PmdVertex{
		float pos[3];
		float normal_vec[3];
		float uv[2];
		unsigned short bone_num[2];
		unsigned char bone_weight;
		unsigned char edge_flag;
	};
	struct PmdMaterial{
		float diffuse_color[3];
		float alpha;
		float specularity;
		float specular_color[3];
		float mirror_color[3];
		unsigned char toon_index;
		unsigned char edge_flag;
		unsigned long face_vert_count;	// ���̍ޗ��̖ʒ��_�� �� �ޗ��ԍ�i�̃|���S���ԍ��F pmdMaterial[i - 1].face_vert_count/3 �` pmdMaterial[i].face_vert_count/3 - 1
		char texture_file_name[20];
	};
	struct PmdBone {
		char bone_name[20];
		unsigned short parent_bone_index;	// �Ȃ��ꍇ��0xFFFF
		unsigned short tail_pos_bone_index;	// �Ȃ��ꍇ��0xFFFF
		unsigned char bone_type;			// 0:��] 1:��]�ƈړ� 2:IK 3:�s�� 4:IK�e���� 5:��]�e���� 6:IK�ڑ��� 7:��\�� 8:�P�� 9:��]�^�� (8, 9��MMD4.0�ȍ~)
		unsigned short ik_parent_bone_index;// �Ȃ��ꍇ��0
		float bone_head_pos[3];
	};
	struct PmdIkDataWithoutArray {
		unsigned short ik_bone_index;			// IK�{�[���ԍ�
		unsigned short ik_target_bone_index;	// IK�{�[���̈ʒu�ɂ��̃{�[������v������悤��IK�������s����
		unsigned char ik_chain_length;			// IK�`�F�[���̒���
		unsigned short iterations;				// �ċA���Z��
		float control_weight;					// �{�[���̒P�ʐ����p 1.0 �� 4.0[rad]�B�܂��u�Ђ��v���܂ރ{�[���������{�[����X�������ɂ��������Ȃ�����������B
		// unsigned short ik_child_bone_index[ik_chain_length]; // IK�e�����̃{�[���ԍ� (�z��̑傫�����ω�)
	};
	struct PmdIkData : public PmdIkDataWithoutArray {
		vector<unsigned short> ik_child_bone_index;
	};


	/// VMD�\���̒�`
	struct VmdHeader {
		char vmdHeader[30];
		char vmdModelName[20];
	};

	struct VmdMotion {
		char boneName[15];
		unsigned long frameNo;
		float location[3];	// �ړ���
		float rotation[4];	// ���f�����[�J�����W�n
		unsigned char interpolation[64];
	};

	/// �X�P�[���ϊ��萔
	const float scale = 0.1f;

#pragma pack(pop)
}