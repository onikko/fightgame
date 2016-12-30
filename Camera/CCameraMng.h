#pragma once
#include "../Utilities/GameObj.h"
class CCameraMng : public CGameObj
{
protected:
	D3DXMATRIX	m_MatView;			//�J�����s��
	D3DXMATRIX	m_MatProjection;	// �v���W�F�N�V�����ϊ��s��
	D3DXMATRIX	m_MatScreen;
	D3DVIEWPORT9 m_port;
public:
	CCameraMng();
	~CCameraMng();
	virtual void SetCamera(){}
	virtual void SetProjection(){}
	virtual void SetViewport(){}
	virtual D3DXMATRIX GetViewMatrix() = 0;
	D3DXMATRIX GetProjectionMatrix() const{ return m_MatProjection; }
	virtual D3DXVECTOR3 InverseScreen(const D3DXVECTOR3& trans) = 0;
};

