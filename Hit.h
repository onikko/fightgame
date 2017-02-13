#pragma once
#include "Utilities/GameObj.h"

class Hit
{
private:
	string* BoneName;
	float* Radius;
	int* BoneIndex;
	D3DXMATRIX* world;
	D3DXVECTOR3* pos;
	int size;
public:
	Hit(int in_size);
	~Hit();
	void SetBoneName(int id, string name){ BoneName[id] = name; }
	string GetBoneName(int id){ return BoneName[id]; }

	void SetRadius(int id, float r){ Radius[id] = r; }
	float GetRadius(int id){ return Radius[id]; }

	void SetBoneIndex(int id, int bone_id){ BoneIndex[id] = bone_id; }
	int GetBoneIndex(int id){ return BoneIndex[id]; }

	void Setworld(int id, D3DXMATRIX in_world){ world[id] = in_world; }
	D3DXMATRIX Getworld(int id){ return world[id]; }

	void Setpos(int id, D3DXVECTOR3 in_pos){ pos[id] = in_pos; }
	D3DXVECTOR3 Getpos(int id){ return pos[id]; }

	int Getsize(){ return size; }
};

class HitA
{
private:
	string BoneName;
	float Radius;
	int BoneIndex;
	D3DXMATRIX world;
	D3DXVECTOR3 pos;
public:
	HitA(){

	}
	~HitA(){

	}
	void SetBoneName(string name){ BoneName = name; }
	string GetBoneName(){ return BoneName; }

	void SetRadius(float r){ Radius = r; }
	float GetRadius(){ return Radius; }

	void SetBoneIndex(int bone_id){ BoneIndex = bone_id; }
	int GetBoneIndex(){ return BoneIndex; }

	void Setworld(D3DXMATRIX in_world){ world = in_world; }
	D3DXMATRIX Getworld(){ return world; }

	void Setpos(D3DXVECTOR3 in_pos){ pos = in_pos; }
	D3DXVECTOR3 Getpos(){ return pos; }

};