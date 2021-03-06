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
	int time;							// 時間
	bool m_isfinished;
	int m_loop_flg;
	bool m_over_flag;
	vector<Bone>* bones;
	vector<MmdStruct::PmdIkData>* pmdIkData;
	/// キーフレームアニメーション
	vector<D3DXQUATERNION> boneRot;					// 現在のボーンの回転
	vector<D3DXVECTOR3> bonePos;					// 現在のボーンの位置
	vector<list<KeyFrame>> keyFrames;				// ボーンごとにキーフレームをリストとしてもつ
	vector<list<KeyFrame>::iterator> ite_keyFrames;	// キーフレームのイテレータ
	/// IK
	void UpdateIK(const MmdStruct::PmdIkData&);		// IKボーン影響下ボーンの行列を更新
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
	int GetTime(){ return time; }
	bool GetOverFlag(){ return m_over_flag; }
	void SetOverFlag(bool in_flag){ m_over_flag = in_flag; }
	void UpdateBoneMatrix();				// キーフレームデータを元にボーン行列を更新
	void AdvanceTime();
};
