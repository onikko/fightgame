#pragma once
#include "Interface.h"
#include "VmdMotionController.h"
#include "PmxStruct.h"
#include "CSkinMeshEffect.h"
#include "../DrawOrder/CGraphicsMeshBone.h"

/// スキンメッシュ
class SkinMesh : public Drawable3D {
protected:
	/// 基本 ///
	SkinMesh(LPDIRECT3DDEVICE9 pDevice);// オブジェクト生成禁止
	SkinMesh(void);// オブジェクト生成禁止
	LPDIRECT3DDEVICE9 pDevice;			// Direct3Dデバイスオブジェクト
	/// メッシュ ///

	LPD3DXMESH pMesh;					// メッシュ
	vector<D3DMATERIAL9> materials;		// マテリアル配列
	vector<LPDIRECT3DTEXTURE9> textures;// テクスチャ配列
	vector<D3DXBONECOMBINATION> boneCombination;// ボーンコンビネーション配列
	virtual LPD3DXSKININFO CreateSkinInfo(const vector<BlendVertex>, const unsigned int, const D3DVERTEXELEMENT9*);	// メッシュ分割のためのスキン情報を作成
	virtual void DivideMesh(unsigned int numFace, LPD3DXMESH pOrgMesh, LPD3DXSKININFO pSkinInfo);					// ブレンド行列パレットのサイズを越えないようにメッシュを分割
	/// ボーン ///
	vector<Bone> bones;					// ボーン構造体
	vector<Bone>* es;
	vector<ID3DXMesh*> boneObj;			// 表示用ボーン
	virtual void CreateBoneObj();													// 表示用ボーンを作成
	virtual vector<D3DXMATRIX> GetWorlds(const D3DXMATRIX* world);					// ボーン行列から変換したワールド変換行列の配列を取得
	/// シェーダ ///
	sp<CSkinMeshEffect> m_effect;
	void CreateShader();	// シェーダの作成
	/// ボーン行列の更新
	virtual void UpdateBoneMatrix() = 0;	// 継承先に実装
public:
	virtual ~SkinMesh();
	virtual void AdvanceTime() = 0;						// 時間をすすめる
	vector<Bone>* GetBoneAddress( void ){
		return &bones;
	}
	LPD3DXMESH GetMesh( void ){
		return pMesh;
	}
	virtual void Draw(D3DXMATRIX world, D3DLIGHT9* light, D3DXMATRIX view, D3DXMATRIX proj);		// 描画
	virtual void DrawBoneObj(D3DXVECTOR3* position, D3DXMATRIX* rotation, D3DLIGHT9* light );// ボーンを表示

	vector<Bone> GetBone( void ){
		return bones;
	}

	int GetBoneId(string name){
		for (unsigned int i = 0; i < bones.size(); ++i){
			if (name == bones[i].name)	return bones[i].id;
		}
		if (name.c_str() == NULL){
			printf("ボーンが見つかりません\n", name.c_str());
			return NULL;
		}
		else{
			printf("%sが見つかりません\n", name.c_str());
			return NULL;
		}
	}
};


/// MMDスキンメッシュ
class MmdSkinMesh sealed : public SkinMesh {
private:
	VmdMotionController* vmdMotionController;
	vector<MmdStruct::PmdIkData> pmdIkData;
	void LoadPmdfile(const LPCTSTR&, vector<MmdStruct::PmdVertex>&, vector<unsigned short>&, vector<MmdStruct::PmdMaterial>&, vector<MmdStruct::PmdBone>&);
	void CreateMesh(vector<MmdStruct::PmdVertex>, vector<unsigned short>, vector<MmdStruct::PmdMaterial>, const unsigned int numBone);
	void CreateBoneMatrix(vector<MmdStruct::PmdBone> pmdBones);
	void CopyMaterial(D3DMATERIAL9&, const MmdStruct::PmdMaterial&);
	void UpdateBoneMatrix() override;
public:
	MmdSkinMesh(LPCTSTR pmdFilename, LPCTSTR vmdFilename, LPDIRECT3DDEVICE9 pDevice);
	~MmdSkinMesh();
	void AdvanceTime();
};

// PMXスキンメッシュ
class PmxSkinMesh sealed : public SkinMesh {
private:
	//vector<PmxStruct::PMX_VERTEX> pmxvertex;
	sp<VmdMotionController> m_spVmdMotionController;
	vector<MmdStruct::PmdIkData> pmxIkData;
	void LoadPmxfile(const LPCTSTR&, PmxStruct::PMX_DATA&);
	void CreateMesh(PmxStruct::PMX_DATA&, string _src );
	void CreateBoneMatrix(PmxStruct::PMX_DATA&);
	void CreateIkData(vector<MmdStruct::PmdIkData>&, PmxStruct::PMX_DATA& );
	void CopyMaterial(D3DMATERIAL9& material, const PmxStruct::PMX_MATERIAL& pmxMaterial);
	void UpdateBoneMatrix() override;
public:
	PmxSkinMesh( void ){};
	void SetDevice( LPDIRECT3DDEVICE9 _device ){
		pDevice = _device;
	}
	/*vector<PmxStruct::PMX_VERTEX>* GetVertex(){
		return &pmxvertex;
	}*/
	void SetMotion( sp<VmdMotionController> sp_motion){
		m_spVmdMotionController = sp_motion;
	}
	vector<MmdStruct::PmdIkData>* GetIkAddress( void ){
		return &pmxIkData;
	}
	PmxSkinMesh( LPDIRECT3DDEVICE9 );
	bool LoadModel(LPCTSTR pmxFilename, LPDIRECT3DDEVICE9 pDevice);
	~PmxSkinMesh();
	void AdvanceTime();
	void SetTech(sp<CSkinMeshEffect>& p1){
		m_effect = p1;
	}
};
