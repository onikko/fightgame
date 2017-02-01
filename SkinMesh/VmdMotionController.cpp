#include "VmdMotionController.h"

using namespace std;

VmdMotionController::VmdMotionController( void ){
};

VmdMotionController::VmdMotionController(LPCTSTR filename, vector<Bone>* b, vector<MmdStruct::PmdIkData>* p) : bones(b), pmdIkData(p), time() {
	const int frame_rate = 60;			// �{�v���O�����̃t���[�����[�g
	const int mmd_frame_rate = 30;		// MMD�̃t���[�����[�g
	// VMD�t�@�C������VMD�f�[�^�𒊏o
    ifstream ifs(filename, ios::binary);
	if (ifs.fail()) throw TEXT("�t�@�C��������܂���");
	MmdStruct::VmdHeader vmdHeader;
	ifs.read((char*)&vmdHeader, sizeof(MmdStruct::VmdHeader));
	unsigned long numVmdMotion;
	ifs.read((char*)&numVmdMotion, sizeof(numVmdMotion));
	vector<MmdStruct::VmdMotion> vmdMotions(numVmdMotion);
	ifs.read((char*)&vmdMotions[0], sizeof(MmdStruct::VmdMotion)*numVmdMotion);
	// KeyFrames�Ɋi�[
	keyFrames.resize(bones->size());
	for (unsigned int i = 0; i < vmdMotions.size(); ++i) {
		KeyFrame keyFrame;
		keyFrame.boneName = vmdMotions[i].boneName;
		keyFrame.frameNo = vmdMotions[i].frameNo;
		keyFrame.frameNo *= frame_rate/mmd_frame_rate;
		keyFrame.position = D3DXVECTOR3(vmdMotions[i].location[0], vmdMotions[i].location[1], vmdMotions[i].location[2]);
		keyFrame.position *= MmdStruct::scale;
		keyFrame.rotation = D3DXQUATERNION(vmdMotions[i].rotation[0], vmdMotions[i].rotation[1], vmdMotions[i].rotation[2], vmdMotions[i].rotation[3]);
		keyFrame.interpolation_x[0] = D3DXVECTOR2(vmdMotions[i].interpolation[0],	vmdMotions[i].interpolation[4]);
		keyFrame.interpolation_x[1] = D3DXVECTOR2(vmdMotions[i].interpolation[8],	vmdMotions[i].interpolation[12]);
		keyFrame.interpolation_y[0] = D3DXVECTOR2(vmdMotions[i].interpolation[1],	vmdMotions[i].interpolation[5]);
		keyFrame.interpolation_y[1] = D3DXVECTOR2(vmdMotions[i].interpolation[9],	vmdMotions[i].interpolation[13]);
		keyFrame.interpolation_z[0] = D3DXVECTOR2(vmdMotions[i].interpolation[2],	vmdMotions[i].interpolation[6]);
		keyFrame.interpolation_z[1] = D3DXVECTOR2(vmdMotions[i].interpolation[10],	vmdMotions[i].interpolation[14]);
		keyFrame.interpolation_r[0] = D3DXVECTOR2(vmdMotions[i].interpolation[3],	vmdMotions[i].interpolation[7]);
		keyFrame.interpolation_r[1] = D3DXVECTOR2(vmdMotions[i].interpolation[11],	vmdMotions[i].interpolation[15]);
		for (unsigned int j = 0; j < bones->size(); ++j) {
			if (keyFrame.boneName == (*bones)[j].name) {	// �{�[��������{�[���ԍ���T��
				keyFrames[j].push_back(keyFrame);
				break;
			}
		}
	}
	for (unsigned int i = 0; i < bones->size(); ++i) {
		keyFrames[i].sort();
		ite_keyFrames.push_back(keyFrames[i].begin());
		boneRot.push_back(D3DXQUATERNION(0, 0, 0, 0));
		bonePos.push_back(D3DXVECTOR3(0, 0, 0));
	}
	UpdateBoneMatrix();
}

bool VmdMotionController::LoadVmdFile(LPCTSTR filename, vector<Bone>* _bones, vector<MmdStruct::PmdIkData>* _pmdIkData, int loopflg )
{	
	const int frame_rate = 60;			// �{�v���O�����̃t���[�����[�g
	const int mmd_frame_rate = 30;		// MMD�̃t���[�����[�g

	bones = _bones;
	pmdIkData = _pmdIkData;
	time = 0;
	m_isfinished = false;
	m_loop_flg = loopflg;
	// VMD�t�@�C������VMD�f�[�^�𒊏o
    ifstream ifs(filename, ios::binary);
	if (ifs.fail()) return false;
	MmdStruct::VmdHeader vmdHeader;
	ifs.read((char*)&vmdHeader, sizeof(MmdStruct::VmdHeader));
	unsigned long numVmdMotion;
	ifs.read((char*)&numVmdMotion, sizeof(numVmdMotion));
	vector<MmdStruct::VmdMotion> vmdMotions(numVmdMotion);
	ifs.read((char*)&vmdMotions[0], sizeof(MmdStruct::VmdMotion)*numVmdMotion);
	// KeyFrames�Ɋi�[
	keyFrames.resize(bones->size());
	for (unsigned int i = 0; i < vmdMotions.size(); ++i) {
		KeyFrame keyFrame;
		keyFrame.boneName = vmdMotions[i].boneName;
		keyFrame.frameNo = vmdMotions[i].frameNo;
		keyFrame.frameNo *= frame_rate/mmd_frame_rate;
		keyFrame.position = D3DXVECTOR3(vmdMotions[i].location[0], vmdMotions[i].location[1], vmdMotions[i].location[2]);
		keyFrame.position *= MmdStruct::scale;
		keyFrame.rotation = D3DXQUATERNION(vmdMotions[i].rotation[0], vmdMotions[i].rotation[1], vmdMotions[i].rotation[2], vmdMotions[i].rotation[3]);
		keyFrame.interpolation_x[0] = D3DXVECTOR2(vmdMotions[i].interpolation[0],	vmdMotions[i].interpolation[4]);
		keyFrame.interpolation_x[1] = D3DXVECTOR2(vmdMotions[i].interpolation[8],	vmdMotions[i].interpolation[12]);
		keyFrame.interpolation_y[0] = D3DXVECTOR2(vmdMotions[i].interpolation[1],	vmdMotions[i].interpolation[5]);
		keyFrame.interpolation_y[1] = D3DXVECTOR2(vmdMotions[i].interpolation[9],	vmdMotions[i].interpolation[13]);
		keyFrame.interpolation_z[0] = D3DXVECTOR2(vmdMotions[i].interpolation[2],	vmdMotions[i].interpolation[6]);
		keyFrame.interpolation_z[1] = D3DXVECTOR2(vmdMotions[i].interpolation[10],	vmdMotions[i].interpolation[14]);
		keyFrame.interpolation_r[0] = D3DXVECTOR2(vmdMotions[i].interpolation[3],	vmdMotions[i].interpolation[7]);
		keyFrame.interpolation_r[1] = D3DXVECTOR2(vmdMotions[i].interpolation[11],	vmdMotions[i].interpolation[15]);
		for (unsigned int j = 0; j < bones->size(); ++j) {
			if (keyFrame.boneName == (*bones)[j].name) {	// �{�[��������{�[���ԍ���T��
				keyFrames[j].push_back(keyFrame);
				break;
			}
		}
	}
	for (unsigned int i = 0; i < bones->size(); ++i) {
		keyFrames[i].sort();
		ite_keyFrames.push_back(keyFrames[i].begin());
		boneRot.push_back(D3DXQUATERNION(0, 0, 0, 0));
		bonePos.push_back(D3DXVECTOR3(0, 0, 0));
	}
	UpdateBoneMatrix();
	return true;
}
void VmdMotionController::UpdateBoneMatrix() {
	bool finish_flg = true;
	for (unsigned int i = 0; i < bones->size(); i++) {
		// �L�[�t���[���⊮
		unsigned long t0, t1;
		D3DXQUATERNION q0, q1;
		D3DXVECTOR3 p0, p1;
		if (ite_keyFrames[i] != keyFrames[i].end()) {
			t0 = (*ite_keyFrames[i]).frameNo;
			boneRot[i] = q0 = (*ite_keyFrames[i]).rotation;
			bonePos[i] = p0 = (*ite_keyFrames[i]).position;
			if (++ite_keyFrames[i] != keyFrames[i].end()) {
				t1 = (*ite_keyFrames[i]).frameNo;
				q1 = (*ite_keyFrames[i]).rotation;
				p1 = (*ite_keyFrames[i]).position;
				float s = (float)(time - t0)/(float)(t1 - t0);	// ���`�⊮
				D3DXQuaternionSlerp(&boneRot[i], &q0, &q1, s);
				bonePos[i] = p0 + (p1 - p0)*s;
				if (time != t1) --ite_keyFrames[i];
				finish_flg = false;
			}
		}
		// �e�{�[�����W�n�̃{�[���s������߂�
		D3DXMATRIX rot, trans;
		D3DXMatrixRotationQuaternion(&rot, &boneRot[i]);
		D3DXMatrixTranslation(&trans, bonePos[i].x, bonePos[i].y, bonePos[i].z);

		(*bones)[i].boneMat = rot*trans*(*bones)[i].initMat;
		// IK�e�����̃{�[�����܂߂ă��Z�b�g���� �� �������s�A���I�ɂȂ�₷��
		//if ((*bones)[i].type != 4) (*bones)[i].boneMat = rot*trans*(*bones)[i].initMat;	// IK�e�����̃{�[�������Z�b�g���Ȃ� �� �������A���I�ɂȂ邪���ԂƋ��ɂ˂���Ă���
	}
	for (unsigned int i = 0; i < pmdIkData->size(); ++i){
		UpdateIK((*pmdIkData)[i]);
	}
	m_isfinished = finish_flg;
	if( finish_flg && m_loop_flg ){
		time = 0;
		for (unsigned int i = 0; i < bones->size(); i++){
			ite_keyFrames[i] = keyFrames[i].begin();
		}
	}
}

void VmdMotionController::UpdateIK(const MmdStruct::PmdIkData& ikData) {

	
	D3DXVECTOR3 localEffectorPos, localTargetPos;

	for (unsigned int j = 0; j < ikData.iterations; ++j) {												// IK�f�[�^�̐������[�v

		for (unsigned int i = 0; i < ikData.ik_child_bone_index.size(); ++i) {								// �q���{�[���̐������[�v

			unsigned short attentionIdx = ikData.ik_child_bone_index[i];									// �h�j�e�����̃{�[���ԍ�

			D3DXVECTOR3 effectorPos = (*bones)[ikData.ik_target_bone_index].GetModelLocalPosition();		// �G�t�F�N�^�̈ʒu

			D3DXVECTOR3 targetPos = (*bones)[ikData.ik_bone_index].GetModelLocalPosition();					// �^�[�Q�b�g�̈ʒu

			D3DXMATRIX invCoord;
			D3DXMatrixInverse(&invCoord, 0, &(*bones)[attentionIdx].GetModelLocalBoneMat());
			D3DXVec3TransformCoord(&localEffectorPos, &effectorPos, &invCoord);						// ���ڃ{�[����ɕϊ��i�G�t�F�N�^�[�j
			D3DXVec3TransformCoord(&localTargetPos, &targetPos, &invCoord);							// ���ڃ{�[����ɕϊ��i�^�[�Q�b�g�j


			D3DXVECTOR3 localEffectorDir;															// �G�t�F�N�^�̃��[�J�������i���ڃ{�[���̈ʒu��j
			D3DXVec3Normalize(&localEffectorDir, &localEffectorPos);

			D3DXVECTOR3 localTargetDir;																// �^�[�Q�b�g�̃��[�J�������i���ڃ{�[���̈ʒu��j
			D3DXVec3Normalize(&localTargetDir, &localTargetPos);

			if ((*bones)[attentionIdx].name.find("�Ђ�") != string::npos) {
				localEffectorDir = D3DXVECTOR3(0, localEffectorDir.y, localEffectorDir.z);
				D3DXVec3Normalize(&localEffectorDir, &localEffectorDir);
				localTargetDir = D3DXVECTOR3(0, localTargetDir.y, localTargetDir.z);
				D3DXVec3Normalize(&localTargetDir, &localTargetDir);
			}

			float p = D3DXVec3Dot(&localEffectorDir, &localTargetDir);
			if (p > 1.0f){
				p = 1.0f;
			}
			if (p < 0.0f){
				p = 0.0f;
			}
			//			if (p > 1 - 1.0e-8f) continue;	// �v�Z�덷�ɂ��1���z�����acos()�����U����̂Œ���!

			// �p�x�����߂�
			float angle = acos(p);

	//			if (angle > 4 * ikData.control_weight) {
	//				angle = 4.0f*ikData.control_weight;
	//			}
			if (angle >  ikData.control_weight) {
				angle = ikData.control_weight;
			}

			D3DXVECTOR3 axis;
			D3DXVec3Cross(&axis, &localEffectorDir, &localTargetDir);

			D3DXMATRIX rotation;
			D3DXMatrixRotationAxis(&rotation, &axis, angle);

			if ((*bones)[attentionIdx].name.find("�Ђ�") != string::npos) {
				D3DXMATRIX inv;
				D3DXMatrixInverse(&inv, 0, &(*bones)[attentionIdx].initMat);

				D3DXMATRIX def = rotation*(*bones)[attentionIdx].boneMat*inv;
				D3DXVECTOR3 t(0, 0, 1);

				D3DXVec3TransformCoord(&t, &t, &def);

				if (t.y < 0){
					D3DXMatrixRotationAxis(&rotation, &axis, -angle);
				}

				// �G�{�[�����G�t�F�N�^�[�i�^�[�Q�b�g�{�[���j���߂��Ƃ��͉�]�ʂ�ǉ�����(
				float l = D3DXVec3Length(&localTargetPos) / D3DXVec3Length(&localEffectorPos);

				if (fabs(angle) <= D3DX_PI / 2 && l < 1.0f){
					static const float a = 0.5f;
					float diff = acosf(l)*a;
					static const float diff_limit = D3DX_PI / 6.0f;
					if (diff > diff_limit){
						diff = diff_limit;
					}
					if (fabs(angle) > 1.0e-6f){
						diff *= angle / fabs(angle);
					}
					angle += diff;
				}
			}

			(*bones)[attentionIdx].boneMat = rotation*(*bones)[attentionIdx].boneMat;
		}

		const float errToleranceSq = 0.000001f;
		if (D3DXVec3LengthSq(&(localEffectorPos - localTargetPos)) < errToleranceSq) {
			return;
		}
	}

}

void VmdMotionController::AdvanceTime() { 
	++time; 
}

