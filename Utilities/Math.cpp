#include "Math.h"

//外積
D3DXVECTOR3 CMath::Vec3Cross(D3DXVECTOR3* out, D3DXVECTOR3* a, D3DXVECTOR3* b)
{
	float ax = a->x, ay = a->y, az = a->z, bx = b->x, by = b->y, bz = b->z;

	out->x = ay * bz - az * by;
	out->y = az * bx - ax * bz;
	out->z = ax * by - ay * bx;
	return *out;
};

//正規化
D3DXVECTOR3 CMath::Normalize(D3DXVECTOR3* out, D3DXVECTOR3* a)
{
	float x = a->x, y = a->y, z = a->z;
	float len = x*x + y*y + z*z;
	if (len > 0) {
		//TODO: evaluate use of glm_invsqrt here?
		len = 1 / sqrtf(len);
		out->x = a->x * len;
		out->y = a->y * len;
		out->z = a->z * len;
	}
	return *out;
};

//クォータニオン
D3DXVECTOR3 CMath::QuaternionRotation(float* theta, D3DXVECTOR3* u, D3DXVECTOR3* v)
{
	D3DXQUATERNION P = { v->x, v->y, v->z, 0 };
	D3DXQUATERNION Q = { -u->x * sin(*theta / 2), -u->y * sin(*theta / 2), -u->z * sin(*theta / 2), cos(*theta / 2) };
	D3DXQUATERNION R = { u->x * sin(*theta / 2), u->y * sin(*theta / 2), u->z * sin(*theta / 2), cos(*theta / 2) };
	D3DXQUATERNION S0;
	S0 = QuaternionMultiply(&S0, &P, &Q);
	D3DXQUATERNION S;
	S = QuaternionMultiply(&S, &R, &S0);
	D3DXVECTOR3 V = { S.x, S.y, S.z };
	return V;
}

//クォータニオン同士の掛け算
D3DXQUATERNION CMath::QuaternionMultiply(D3DXQUATERNION* out, D3DXQUATERNION* a, D3DXQUATERNION* b)
{
	float ax = a->x, ay = a->y, az = a->z, aw = a->w, bx = b->x, by = b->y, bz = b->z, bw = b->w;

	out->x = ax * bw + aw * bx + ay * bz - az * by;
	out->y = ay * bw + aw * by + az * bx - ax * bz;
	out->z = az * bw + aw * bz + ax * by - ay * bx;
	out->w = aw * bw - ax * bx - ay * by - az * bz;
	return *out;
};

//積算行列の計算
void CMath::MakeTotalMatrix(D3DXMATRIX& mat, D3DXMATRIX& mattotal, const D3DXVECTOR3& angle, const D3DXVECTOR3& trans)
{
	D3DXMATRIX	matx, maty, matz;

	D3DXMatrixRotationX(&matx, (float)(angle.x / 180 * D3DX_PI));		//	X軸回転値をセット
	D3DXMatrixRotationY(&maty, (float)(angle.y / 180 * D3DX_PI));		//	Y軸回転値をセット
	D3DXMatrixRotationZ(&matz, (float)(angle.z / 180 * D3DX_PI));		//	Z軸回転値をセット

	mat = matx*maty*matz;						// 行列を合成	

	mat._41 = trans.x;				// Ｘ平行移動成分をセット
	mat._42 = trans.y;				// Ｙ平行移動成分をセット
	mat._43 = trans.z;				// Ｚ平行移動成分をセット

	mattotal = mat * mattotal;		// 行列を積算
}

//行列の計算
void CMath::MakeWorldMatrix(D3DXMATRIX& mat, const D3DXVECTOR3& angle, const D3DXVECTOR3& trans)
{
	D3DXMATRIX	matx, maty, matz;

	D3DXMatrixRotationX(&matx, (float)(angle.x / 180 * D3DX_PI));		//	X軸回転値をセット
	D3DXMatrixRotationY(&maty, (float)(angle.y / 180 * D3DX_PI));		//	Y軸回転値をセット
	D3DXMatrixRotationZ(&matz, (float)(angle.z / 180 * D3DX_PI));		//	Z軸回転値をセット

	mat = matx*maty*matz;						// 行列を合成	

	mat._41 = trans.x;				// Ｘ平行移動成分をセット
	mat._42 = trans.y;				// Ｙ平行移動成分をセット
	mat._43 = trans.z;				// Ｚ平行移動成分をセット

}