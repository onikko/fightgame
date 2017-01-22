#include "CamaraTPS.h"
#include "../Utilities/Input.h"

// �J�����̉�]���x
#define CAMERA_ANGLE_SPEED 3.0f
// �J�����̒����_�̍���
#define CAMERA_LOOK_AT_HEIGHT 1.7f
#define CAMERA_DISTANCE 5.0f
#define MIN_DIST 1
#define VIEW_FROM_Y 2
#define VIEW_LOOKAT_Y 1
#define VIEW_DIST_MIN 5.5f
#define VIEW_DIST_MAX 10.0f
#define VIEW_DIST_RATE 0.3f

CCamaraTPS::CCamaraTPS()
{
	printf("�J������ǂݍ��݂܂���\n");
}


CCamaraTPS::~CCamaraTPS()
{
	printf("�J������j�����܂���\n");
}

void CCamaraTPS::SetViewport()
{
	m_port = { 0, 0, (DWORD)m_width, (DWORD)m_height, 0.0f, 1.0f };
	m_lpd3ddevice->SetViewport(&m_port);	//�`��̈��ύX�i�r���[�|�[�g�s��j
}

void CCamaraTPS::SetProjection(){
	// �v���W�F�N�V�����ϊ��s��쐬
	D3DXMatrixPerspectiveFovLH(&m_MatProjection, D3DX_PI / 4, (float)m_width / (float)m_height, 1.0f, 100000.0f);
	m_lpd3ddevice->SetTransform(D3DTS_PROJECTION, &m_MatProjection);
}

void CCamaraTPS::SetCamera(){
	m_lpd3ddevice->SetTransform(D3DTS_VIEW, &m_MatView);	// �J�����s����O���t�B�b�N�J�[�h�ɃZ�b�g
}

void CCamaraTPS::Init()
{
	player = (CPlayer*)FindItemBox("player");
	enemy = (CEnemy*)FindItemBox("enemy");
	m_Pos = { 0, 0, 0 };
	m_Lookat = { 0, 0, 0 };
	m_Up = { 0, 1, 0 };
	m_Camera_Distance = CAMERA_DISTANCE;
	m_CameraVAngle = 0.0f;
	m_CameraHAngle = 0.0f;
	m_SinParam = 0.0f;
	m_CosParam = 0.0f;
	SetProjection();
	GetScreenMatrix();
}

void CCamaraTPS::Input()
{
	if (CInput::Instance().CheckKeyBuffer(DIK_UP))
	{
		m_CameraVAngle += CAMERA_ANGLE_SPEED;
		if (m_CameraVAngle >= 80.0f)
		{
			m_CameraVAngle = 80.0f;
		}
	}

	if (CInput::Instance().CheckKeyBuffer(DIK_DOWN))
	{
		m_CameraVAngle -= CAMERA_ANGLE_SPEED;
		if (m_CameraVAngle <= -5.0f)
		{
			m_CameraVAngle = -5.0f;
		}
	}

	if (CInput::Instance().CheckKeyBuffer(DIK_RIGHT))
	{
		m_CameraHAngle += CAMERA_ANGLE_SPEED;
		if (m_CameraHAngle >= 180.0f)
		{
			m_CameraHAngle -= 360.0f;
		}
	}

	if (CInput::Instance().CheckKeyBuffer(DIK_LEFT))
	{
		m_CameraHAngle -= CAMERA_ANGLE_SPEED;
		if (m_CameraHAngle <= -180.0f)
		{
			m_CameraHAngle += 360.0f;
		}
	}

	if (CInput::Instance().CheckKeyBufferTrigger(DIK_F1)){
		m_Camera_Distance = 4.0f;
	}
}

void CCamaraTPS::Update()
{
	// �����̏���
	D3DXMATRIX& m0 = player->GetMatrixWorld();
	D3DXMATRIX& m1 = enemy->GetMatrixWorld();
	D3DXVECTOR3 p0 = D3DXVECTOR3(m0._41, 0, 0);
	D3DXVECTOR3 p1 = D3DXVECTOR3(m1._41, 0, 0);
	D3DXVECTOR3 center = (p0 + p1)*0.5f;
	D3DXVECTOR3 unit = p0 - p1;
	float dist = D3DXVec3Length(&unit);
	D3DXVec3Normalize(&unit, &unit);

	// �J��������
	if (dist >= MIN_DIST*0.5f) {
		float view_dist = dist*VIEW_DIST_MIN*VIEW_DIST_RATE;
		if (view_dist<VIEW_DIST_MIN) view_dist = VIEW_DIST_MIN;
		if (view_dist>VIEW_DIST_MAX) view_dist = VIEW_DIST_MAX;
		D3DXVECTOR3 view_from[] = {
			D3DXVECTOR3(center.x - unit.z*view_dist, VIEW_FROM_Y, center.z - unit.x*view_dist),
			D3DXVECTOR3(center.x + unit.z*view_dist, VIEW_FROM_Y, center.z + unit.x*view_dist)
		};
		float d[2];
		for (int i = 0; i<2; i++) {
			D3DXVECTOR3 v = m_Pos - view_from[i];
			d[i] = D3DXVec3Length(&v);
		}
		m_Pos = view_from[d[0]<d[1] ? 0 : 1];
		m_Lookat = { center.x, VIEW_LOOKAT_Y, center.z };
		m_Up = { 0, 1, 0 };
		if (m_TopView) {
			m_Pos = D3DXVECTOR3(center.x, view_dist*1.2f, center.z);
			m_Lookat = D3DXVECTOR3(center.x, 0, center.z);
			m_Up = D3DXVECTOR3(unit.z, 0, -unit.x);
		}

		//D3DXVECTOR3 TempPosition1;
		//D3DXVECTOR3 TempPosition2;

		//// �����_�̓L�����N�^�[���f���̍��W���� CAMERA_LOOK_AT_HEIGHT �����������ʒu
		//m_Lookat.x = 0;
		//m_Lookat.y = CAMERA_LOOK_AT_HEIGHT;

		//// �J�����̈ʒu�̓J�����̐����p�x�Ɛ����p�x����Z�o

		//// �ŏ��ɐ����p�x�𔽉f�����ʒu���Z�o
		//m_SinParam = sin(m_CameraVAngle / 180.0f * D3DX_PI);
		//m_CosParam = cos(m_CameraVAngle / 180.0f * D3DX_PI);
		//TempPosition1.x = 0.0f;
		//TempPosition1.y = m_SinParam * m_Camera_Distance;
		//TempPosition1.z = -m_CosParam * m_Camera_Distance;

		//// ���ɐ����p�x�𔽉f�����ʒu���Z�o
		//m_SinParam = sin(m_CameraHAngle / 180.0f * D3DX_PI);
		//m_CosParam = cos(m_CameraHAngle / 180.0f * D3DX_PI);
		//TempPosition2.x = m_CosParam * TempPosition1.x - m_SinParam * TempPosition1.z;
		//TempPosition2.y = TempPosition1.y;
		//TempPosition2.z = m_SinParam * TempPosition1.x + m_CosParam * TempPosition1.z;

		//// �Z�o�������W�ɒ����_�̈ʒu�����Z�������̂��J�����̈ʒu
		//m_Pos.x = TempPosition2.x + m_Lookat.x;
		//m_Pos.y = TempPosition2.y + m_Lookat.y;
		//m_Pos.z = TempPosition2.z + m_Lookat.z;


		D3DXMatrixLookAtLH(&m_MatView, &m_Pos, &m_Lookat, &m_Up);
	}

	//// �J�����̐ݒ�ɔ��f����
	//D3DXMatrixLookAtLH(&m_MatView, &m_Pos, &m_Lookat, &m_Up);
}

void CCamaraTPS::Render()
{
	SetViewport();
	SetCamera();
}


D3DXVECTOR3 CCamaraTPS::InverseScreen(const D3DXVECTOR3& trans)
{
	D3DXMATRIX matViewToScreen;
	D3DXVECTOR3 out;
	D3DXMatrixMultiply(&matViewToScreen, &m_MatView, &m_MatProjection);
	D3DXMatrixMultiply(&matViewToScreen, &matViewToScreen, &m_MatScreen);
	D3DXMatrixInverse(&matViewToScreen, NULL, &matViewToScreen);
	D3DXVec3TransformCoord(&out, &trans, &matViewToScreen);
	return out;
}

void CCamaraTPS::GetScreenMatrix()
{
	float width = (float)m_width;
	float height = (float)m_height;
	D3DXMatrixIdentity(&m_MatScreen);
	width /= 2.0f;
	height /= 2.0f;
	m_MatScreen._11 = width;
	m_MatScreen._22 = -height;
	m_MatScreen._41 = width;
	m_MatScreen._42 = height;
}