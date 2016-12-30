#pragma once
#include <d3dx9.h>

struct HITDATA{
	float minx, miny, minz;	//�o�E���f�B���O�{�b�N�X�ŏ��l
	float maxx, maxy, maxz;	//�o�E���f�B���O�{�b�N�X�ő�l
	float cx, cy, cz;		//�o�E���f�B���O�X�t�B�A���S���W
	float r;				//�o�E���f�B���O�X�t�B�A���a
};

class CBoundingSphere
{
private:
	HITDATA m_hitdata;		//�����蔻��p�f�[�^
	D3DXVECTOR3 m_wpos;		//���[���h��ԏ�̈ʒu���W
	D3DXMATRIX m_matrix;	//���E���̕`��p�s��
	D3DMATERIAL9 m_material;//�������}�e���A��
	LPD3DXMESH m_spheremesh;//�����b�V��

	void TraneparentMaterial(float alpha, D3DXCOLOR color){
		//�������ȃ}�e���A���𐶐�
		m_material.Diffuse.r = 1.0f;
		m_material.Diffuse.g = 1.0f;
		m_material.Diffuse.b = 1.0f;
		m_material.Diffuse.a = alpha;
		
		m_material.Ambient.r = color.r;
		m_material.Ambient.g = color.g;
		m_material.Ambient.b = color.b;
		m_material.Ambient.a = color.a;

		m_material.Specular.r = 1.0f;
		m_material.Specular.g = 1.0f;
		m_material.Specular.b = 1.0f;
		m_material.Specular.a = 1.0f;

		m_material.Emissive.r = 0.0f;
		m_material.Emissive.g = 0.0f;
		m_material.Emissive.b = 0.0f;
		m_material.Emissive.a = 0.0f;
	}
	void CalculateBoundingSphere(const LPD3DXMESH mesh, const LPDIRECT3DDEVICE9 lpdev);
public:
	CBoundingSphere(){
		m_spheremesh = nullptr;
	}
	~CBoundingSphere(){
		Exit();
	}

	void Init(const LPD3DXMESH mesh, const LPDIRECT3DDEVICE9 lpdev, float r, D3DXCOLOR color){
		TraneparentMaterial(0.55f, color);				//�����x�̐ݒ�
		CalculateBoundingSphere(mesh, lpdev);	//�q�b�g�f�[�^�̌v�Z
		//CreateSphereMesh(lpdev, m_hitdata.r);	//�����b�V���𐶐�
		CreateSphereMesh(lpdev, r);
	}

	void CreateSphereMesh(const LPDIRECT3DDEVICE9 lpdev, float r){
		//�o�E���f�B���O�X�t�B�A��\�������b�V���𐶐�
		D3DXCreateSphere(lpdev, r, 16, 16, &m_spheremesh, nullptr);
	}

	void UpdatePosition(const D3DXMATRIX mat){
		D3DXVECTOR3 pos(m_hitdata.cx, m_hitdata.cy, m_hitdata.cz);
		//�s��x�x�N�g���i���[���h���W�ɕϊ��j
		D3DXVec3TransformCoord(&m_wpos, &pos, &mat);
		m_matrix = mat;
		m_matrix._41 = m_wpos.x;
		m_matrix._42 = m_wpos.y;
		m_matrix._43 = m_wpos.z;
	}

	void UpdatePosition(const D3DXMATRIX mat, float x, float y, float z){
		//D3DXVECTOR3 pos(m_hitdata.cx, m_hitdata.cy, m_hitdata.cz);
		D3DXVECTOR3 pos(x, y, z);
		//�s��x�x�N�g���i���[���h���W�ɕϊ��j
		D3DXVec3TransformCoord(&m_wpos, &pos, &mat);
		m_matrix = mat;
		m_matrix._41 = m_wpos.x;
		m_matrix._42 = m_wpos.y;
		m_matrix._43 = m_wpos.z;
	}

	void Exit(){
		if (m_spheremesh){
			m_spheremesh->Release();
			m_spheremesh = nullptr;
		}
	}

	void Draw(LPDIRECT3DDEVICE9 lpdev){
		lpdev->SetMaterial(&m_material);
		lpdev->SetTexture(0, nullptr);
		lpdev->SetTransform(D3DTS_WORLD, &m_matrix);
		m_spheremesh->DrawSubset(0);
	}

	float Get() const{
		return m_hitdata.r;
	}

	D3DXVECTOR3& GetWPos(){
		return m_wpos;
	}

	HITDATA Hit() const{
		return m_hitdata;
	}

};

