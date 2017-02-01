#include "HitBox.h"

#define		VERTEX_FVF	(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

bool CHitBox::LoadXFile(LPCTSTR xfilename)
{
	LPD3DXBUFFER	pd3dxmtrlbuffer;
	LPD3DXBUFFER	pd3dxmtrlbuffer2;
	HRESULT			hr;
	unsigned int	i;

	// Ｘファイルを読み込む
	hr = D3DXLoadMeshFromX(xfilename,				// Ｘファイル名
		D3DXMESH_SYSTEMMEM,		// 読み込みメモリ
		m_lpd3ddevice,			// デバイスオブジェクト
		NULL,
		&pd3dxmtrlbuffer,		// マテリアルバッファ
		&pd3dxmtrlbuffer2,		// マテリアルバッファ2
		&m_nummaterial,			// マテリアル数
		&m_lpmesh);				// メッシュ

	if (SUCCEEDED(hr)){
		D3DXMATERIAL	*d3dxmaterials = (D3DXMATERIAL*)pd3dxmtrlbuffer->GetBufferPointer();	// マテリアルのアドレスを取得
		m_lpmeshmaterials = new D3DMATERIAL9[m_nummaterial];		// マテリアルコンテナの作成
		m_lpmeshtextures = new LPDIRECT3DTEXTURE9[m_nummaterial];	// テクスチャコンテナの生成

		for (i = 0; i<m_nummaterial; i++){
			m_lpmeshmaterials[i] = d3dxmaterials[i].MatD3D;
			m_lpmeshmaterials[i].Emissive = m_lpmeshmaterials[i].Specular = m_lpmeshmaterials[i].Ambient = m_lpmeshmaterials[i].Diffuse;

			hr = D3DXCreateTextureFromFile(m_lpd3ddevice,
				d3dxmaterials[i].pTextureFilename,
				&m_lpmeshtextures[i]);
			if (FAILED(hr)){
				m_lpmeshtextures[i] = NULL;
			}
		}
		pd3dxmtrlbuffer->Release();		// マテリアルバッファのリリース
		pd3dxmtrlbuffer2->Release();	// マテリアルバッファのリリース
	}
	else{
		return false;
	}
	return true;
}

//==============================================================================
//!	@fn		UnLoadXFile
//!	@brief	Xファイルオブジェクトアンロード
//!	@param	
//!	@retval	なし
//!	@note	
//==============================================================================
void CHitBox::UnLoadXFile()
{
	unsigned int i;

	if (m_lpmeshmaterials != NULL){	// マテリアルオブジェクトの解放
		delete[] m_lpmeshmaterials;
		m_lpmeshmaterials = NULL;
	}

	if (m_lpmeshtextures != NULL){		// テクスチャオブジェクトの解放
		for (i = 0; i<m_nummaterial; i++){
			if (m_lpmeshtextures[i] != NULL)	m_lpmeshtextures[i]->Release();
		}
		delete[] m_lpmeshtextures;
		m_lpmeshtextures = NULL;
	}

	if (m_lpmesh != NULL){		// メッシュ解放
		m_lpmesh->Release();
		m_lpmesh = NULL;
	}
}

void CHitBox::Update(D3DXMATRIX& matworld,D3DXMATRIX mat, D3DXVECTOR3 pos, float r)
{
	D3DXMATRIX scale;
	D3DXVECTOR3 m_wpos;
	D3DXMatrixScaling(&scale, r, r, r);
	D3DXVec3TransformCoord(&m_wpos, &pos, &mat);
	matworld = scale*mat;
	matworld._41 = m_wpos.x;
	matworld._42 = m_wpos.y;
	matworld._43 = m_wpos.z;
}

// 変換行列を指定して描画
void CHitBox::Draw(const D3DXMATRIX& mat) {
	m_lpd3ddevice->SetTransform(D3DTS_WORLD, &mat);
	Draw();
}

void CHitBox::Draw() {
	D3DMATERIAL9 mat;
	unsigned int i;
	for (i = 0; i < m_nummaterial; i++){
		mat = m_lpmeshmaterials[i];
		D3DXCOLOR* col;
#define COLOR_OPERATION(TARGET) \
			col=(D3DXCOLOR*)&mat.TARGET;\
			D3DXColorModulate(col, col, &ColorMultiplier);\
			D3DXColorAdd(col, col, &ColorAddition);
		COLOR_OPERATION(Diffuse);
		COLOR_OPERATION(Ambient);
		COLOR_OPERATION(Specular);
		COLOR_OPERATION(Emissive);
		m_lpd3ddevice->SetMaterial(&mat);						// マテリアルのセット
		m_lpd3ddevice->SetTexture(0, m_lpmeshtextures[i]);		// テクスチャのセット
		m_lpmesh->DrawSubset(i);								// サブセットの描画
	}
}

LPD3DXMESH CHitBox::GetMesh() const{
	return m_lpmesh;
}


//sample::sample()
//{
//	if (FAILED(m_lpd3ddevice->CreateVertexBuffer(sizeof(Vertex) * 4, 0, VERTEX_FVF, D3DPOOL_MANAGED, &buffer, 0))) {
//		return;
//	}
//	Vertex *p;
//	buffer->Lock(0, 0, (void**)&p, 0);
//	{
//		memcpy(p, billboard, sizeof(Vertex) * 4);
//		buffer->Unlock();
//	}
//}
//
//sample::~sample()
//{
//	SAFE_RELEASE(buffer);
//}
//
//bool sample::LoadXFile()
//{
//	if (FAILED(m_lpd3ddevice->CreateVertexBuffer(sizeof(Vertex) * 4, 0, VERTEX_FVF, D3DPOOL_MANAGED, &buffer, 0))) {
//		return false;
//	}
//	Vertex *p;
//	buffer->Lock(0, 0, (void**)&p, 0);
//	{
//		memcpy(p, billboard, sizeof(Vertex) * 4);
//		buffer->Unlock();
//	}
//
//	player = (CPlayer*)FindItemBox("player");
//
//	return true;
//}
//
//void sample::UnLoadXFile()
//{
//	SAFE_RELEASE(buffer);
//}
//
//void sample::Update()
//{
//	D3DXVECTOR3 pos;
//	D3DXMatrixIdentity(&worldMat);
//	pos = { 0.0f, 0.0f, 0.0f };
//	D3DXVec3TransformCoord(&m_wpos, &pos, &player->GetMatrixWorld());
//	D3DXMatrixRotationY(&maty, (float)(90 / 180 * D3DX_PI));
//	worldMat = maty;
//	worldMat._11 = 0.2f;
//	worldMat._22 = 1.2f;
//	worldMat._41 = m_wpos.x;
//	worldMat._42 = m_wpos.y;
//	worldMat._43 = m_wpos.z;
//	for (int i = 0; i < 4; i++){
//		billboard[i].diffuse = D3DCOLOR_ARGB(100, 0, 255, 0);
//	}
//
//	// 行列設定
//	m_lpd3ddevice->SetTransform(D3DTS_WORLD, &worldMat);
//}
//
//void sample::Draw()
//{
//	m_lpd3ddevice->SetRenderState(D3DRS_LIGHTING, FALSE);
//	m_lpd3ddevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
//
//	// 板ポリを描画
//	m_lpd3ddevice->SetTexture(0, NULL);
//	m_lpd3ddevice->SetStreamSource(0, buffer, 0, sizeof(Vertex));
//	m_lpd3ddevice->SetFVF(VERTEX_FVF);
//	m_lpd3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
//
//	m_lpd3ddevice->SetRenderState(D3DRS_LIGHTING, TRUE);
//	m_lpd3ddevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
//}