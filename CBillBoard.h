#pragma once
#include	<d3dx9.h>
/*----------------------------------------------------------------------

	ビルボードクラス	

-----------------------------------------------------------------------*/
#define		D3DFVFCUSTOM_VERTEX	(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )			// 頂点フォーマット

class CBillBoard{
	D3DXMATRIX			m_mat;				// ビルボード用の行列
	float				m_x;				// ビルボードの位置座標
	float				m_y;					
	float				m_z;
	LPDIRECT3DTEXTURE9	m_Tex1;				// テクスチャ
	float				m_XSize;			// ビルボードのＸサイズ
	float				m_YSize;			// ビルボードのＹサイズ
	float				m_XSizeMAX;			// ビルボードのＸサイズ
	float				m_YSizeMAX;			// ビルボードのＹサイズ
	D3DCOLOR			m_Color;			// カラー値

	// 頂点フォーマット
	struct MyVertex{
		float		x,y,z;
		D3DCOLOR	color;
		float		tu,tv;
	};

	MyVertex		m_Vertex[4];			// ビルボードの頂点座標
private:

	// ビルボード用の行列を生成
	void CalcBillBoardMatrix(const D3DXMATRIX& cameramat);

	// 描画
	void Draw(LPDIRECT3DDEVICE9 lpDXDevice);
public:
	CBillBoard():m_x(0),m_y(0),m_z(0),m_Tex1(NULL){
	}

	// コンストラクタ
	CBillBoard(float x, float y, float z, float xsize, float ysize, float xsizemax, float ysizemax, D3DCOLOR color)
		:m_x(x), m_y(y), m_z(z), m_XSize(xsize), m_YSize(ysize), m_XSizeMAX(xsizemax), m_YSizeMAX(ysizemax), m_Tex1(NULL), m_Color(color){
		CalcVertex();
	}

	// デストラクタ
	virtual ~CBillBoard(){
		if(m_Tex1!=NULL){
			m_Tex1->Release();
		}
	}
	// ビルボードの頂点座標を計算
	void CalcVertex();
	void InitCalcVertex(int num, float x_size, float y_size);
	// 位置を指定
	void SetPosition(float x,float y,float z);

	void Draw(LPDIRECT3DDEVICE9 lpDXDevice, const D3DXMATRIX& cameramat, LPDIRECT3DTEXTURE9 d3dTex);

	// ビルボードを描画
	void DrawBillBoard(LPDIRECT3DDEVICE9 lpDXDevice,const D3DXMATRIX& cameramat);

	// ビルボードをZ軸を中心にして回転させて描画
	void DrawRotateBillBoard(LPDIRECT3DDEVICE9 lpDXDevice, const D3DXMATRIX &cameramat, float radian);
	
	// サイズをセット
	void SetSize(float x, float y, float x_max, float y_max);

	// カラーをセット
	void SetColor(D3DCOLOR col);

	//	テクスチャ読み込み
	bool LoadTexTure(LPDIRECT3DDEVICE9 lpDXDevice,const char* filename);
};