#pragma once
#include "MmdStruct.h"
#include "SkinMeshStruct.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <list>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

class VmdMotionController sealed {
private:
	int time;							// ����
	bool m_isfinished;
	int m_loop_flg;
	vector<Bone>* bones;
	vector<MmdStruct::PmdIkData>* pmdIkData;
	/// �L�[�t���[���A�j���[�V����
	vector<D3DXQUATERNION> boneRot;					// ���݂̃{�[���̉�]
	vector<D3DXVECTOR3> bonePos;					// ���݂̃{�[���̈ʒu
	vector<list<KeyFrame>> keyFrames;				// �{�[�����ƂɃL�[�t���[�������X�g�Ƃ��Ă���
	vector<list<KeyFrame>::iterator> ite_keyFrames;	// �L�[�t���[���̃C�e���[�^
	/// IK
	void UpdateIK(const MmdStruct::PmdIkData&);		// IK�{�[���e�����{�[���̍s����X�V
public:
	VmdMotionController(LPCTSTR filename, vector<Bone>* bones, vector<MmdStruct::PmdIkData>* pmdIkData);
	VmdMotionController( void );
	bool LoadVmdFile(LPCTSTR filename, vector<Bone>* bones, vector<MmdStruct::PmdIkData>* pmdIkData, int _loopflg );
	void ResetTime(){
		time = 0;
		m_isfinished = false;
		for (unsigned int i = 0; i < bones->size(); i++){
			ite_keyFrames[i] = keyFrames[i].begin();
		}
	}
	int IsFinished( void ){
		return m_isfinished;
	}
	void UpdateBoneMatrix();				// �L�[�t���[���f�[�^�����Ƀ{�[���s����X�V
	void AdvanceTime();
};
