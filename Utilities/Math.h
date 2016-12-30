#pragma once
#include "GameObj.h"

class CMath
{
public:
	D3DXVECTOR3 Vec3Cross(D3DXVECTOR3* out, D3DXVECTOR3* a, D3DXVECTOR3* b);
	D3DXVECTOR3 Normalize(D3DXVECTOR3* out, D3DXVECTOR3* a);
	D3DXVECTOR3 QuaternionRotation(float* theta, D3DXVECTOR3* u, D3DXVECTOR3* v);
	D3DXQUATERNION QuaternionMultiply(D3DXQUATERNION* out, D3DXQUATERNION* a, D3DXQUATERNION* b);
	void MakeTotalMatrix(D3DXMATRIX& mat, D3DXMATRIX& mattotal, const D3DXVECTOR3& angle, const D3DXVECTOR3& trans);
	void MakeWorldMatrix(D3DXMATRIX& mat, const D3DXVECTOR3& angle, const D3DXVECTOR3& trans);
};

