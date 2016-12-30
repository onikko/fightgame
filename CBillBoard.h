#pragma once
#include	<d3dx9.h>
/*----------------------------------------------------------------------

	�r���{�[�h�N���X	

-----------------------------------------------------------------------*/
#define		D3DFVFCUSTOM_VERTEX	(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )			// ���_�t�H�[�}�b�g

class CBillBoard{
	D3DXMATRIX			m_mat;				// �r���{�[�h�p�̍s��
	float				m_x;				// �r���{�[�h�̈ʒu���W
	float				m_y;					
	float				m_z;
	LPDIRECT3DTEXTURE9	m_Tex1;				// �e�N�X�`��
	float				m_XSize;			// �r���{�[�h�̂w�T�C�Y
	float				m_YSize;			// �r���{�[�h�̂x�T�C�Y
	float				m_XSizeMAX;			// �r���{�[�h�̂w�T�C�Y
	float				m_YSizeMAX;			// �r���{�[�h�̂x�T�C�Y
	D3DCOLOR			m_Color;			// �J���[�l

	// ���_�t�H�[�}�b�g
	struct MyVertex{
		float		x,y,z;
		D3DCOLOR	color;
		float		tu,tv;
	};

	MyVertex		m_Vertex[4];			// �r���{�[�h�̒��_���W
private:

	// �r���{�[�h�p�̍s��𐶐�
	void CalcBillBoardMatrix(const D3DXMATRIX& cameramat);

	// �`��
	void Draw(LPDIRECT3DDEVICE9 lpDXDevice);
public:
	CBillBoard():m_x(0),m_y(0),m_z(0),m_Tex1(NULL){
	}

	// �R���X�g���N�^
	CBillBoard(float x, float y, float z, float xsize, float ysize, float xsizemax, float ysizemax, D3DCOLOR color)
		:m_x(x), m_y(y), m_z(z), m_XSize(xsize), m_YSize(ysize), m_XSizeMAX(xsizemax), m_YSizeMAX(ysizemax), m_Tex1(NULL), m_Color(color){
		CalcVertex();
	}

	// �f�X�g���N�^
	virtual ~CBillBoard(){
		if(m_Tex1!=NULL){
			m_Tex1->Release();
		}
	}
	// �r���{�[�h�̒��_���W���v�Z
	void CalcVertex();
	void InitCalcVertex(int num, float x_size, float y_size);
	// �ʒu���w��
	void SetPosition(float x,float y,float z);

	void Draw(LPDIRECT3DDEVICE9 lpDXDevice, const D3DXMATRIX& cameramat, LPDIRECT3DTEXTURE9 d3dTex);

	// �r���{�[�h��`��
	void DrawBillBoard(LPDIRECT3DDEVICE9 lpDXDevice,const D3DXMATRIX& cameramat);

	// �r���{�[�h��Z���𒆐S�ɂ��ĉ�]�����ĕ`��
	void DrawRotateBillBoard(LPDIRECT3DDEVICE9 lpDXDevice, const D3DXMATRIX &cameramat, float radian);
	
	// �T�C�Y���Z�b�g
	void SetSize(float x, float y, float x_max, float y_max);

	// �J���[���Z�b�g
	void SetColor(D3DCOLOR col);

	//	�e�N�X�`���ǂݍ���
	bool LoadTexTure(LPDIRECT3DDEVICE9 lpDXDevice,const char* filename);
};