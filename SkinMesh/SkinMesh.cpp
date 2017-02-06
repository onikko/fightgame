#include "../Utilities/SafeDelete.h"

#include "../Factory.h"
#include "SkinMesh.h"

#include "CLoadPMX.h"
#include "CPathControll.h"
#include "DXErr.h"
#include "../Camera/CamaraTPS.h"

namespace {
	const unsigned long palleteSize = 50;	// 行列パレットのサイズ
}

///// SkinMeshクラス /////
SkinMesh::SkinMesh(LPDIRECT3DDEVICE9 pDev) : pDevice(pDev), pMesh(0) {
}

SkinMesh::SkinMesh(void) : pDevice(NULL), pMesh(0) {
}


SkinMesh::~SkinMesh(){
	for (unsigned long i = 0; i < boneCombination.size(); ++i) delete[] boneCombination[i].BoneId;
	for (unsigned int i = 0; i < textures.size(); ++i) SAFE_RELEASE(textures[i]);
	SAFE_RELEASE(pMesh);
}

LPD3DXSKININFO SkinMesh::CreateSkinInfo(const vector<BlendVertex> blendVertices, const unsigned int numBone, const D3DVERTEXELEMENT9* declAry) {
	LPD3DXSKININFO pSkinInfo;
	unsigned long numVertex = blendVertices.size();
	D3DXCreateSkinInfo(numVertex, declAry, numBone, &pSkinInfo);
	for (unsigned int i = 0; i < numBone; ++i) {
		unsigned long numInfluence = 0;								// ボーンiに影響する頂点の数
		unsigned long* vertices = new unsigned long[4*numVertex];	// ボーンiに影響する頂点の配列
		float* weights = new float[4*numVertex];					// ボーンiに影響する重みの配列
		for (unsigned long j = 0; j < numVertex; ++j) {
			for (unsigned int k = 0; k < 4; ++k) {
				if (blendVertices[j].matrixIndex[k] == i) {
					vertices[numInfluence] = j;
					if (k < 3) weights[numInfluence] = blendVertices[j].weight[k];
					else {
						weights[numInfluence] = 1.0;
						for (unsigned int l = 0; l < 3; ++l) weights[numInfluence] -= blendVertices[j].weight[l];
					}
					++numInfluence;
				}
			}
		}
		if(FAILED(pSkinInfo->SetBoneInfluence(i, numInfluence, vertices, weights))) throw TEXT("SkinInfoの作成に失敗しました");;
		delete[] vertices;
		delete[] weights;
	}
	return pSkinInfo;
}

void SkinMesh::DivideMesh(unsigned int numFace, LPD3DXMESH pOrgMesh, LPD3DXSKININFO pSkinInfo) {
	unsigned long* pAdjacencyIn = new unsigned long[3*numFace];
	if (FAILED(pOrgMesh->GenerateAdjacency(0, pAdjacencyIn))) throw TEXT("隣接性情報の作成に失敗しました");
	unsigned long maxFaceInfl;
	unsigned long numBoneCombination;
	LPD3DXBUFFER pBoneCombinationBuffer = 0;
	if(FAILED(pSkinInfo->ConvertToIndexedBlendedMesh(pOrgMesh, 0, palleteSize, pAdjacencyIn, 0, 0, 0, &maxFaceInfl, &numBoneCombination, &pBoneCombinationBuffer, &pMesh))) throw TEXT("メッシュの分割に失敗しました");
	LPD3DXBONECOMBINATION bc = (LPD3DXBONECOMBINATION)pBoneCombinationBuffer->GetBufferPointer();
	for (unsigned long i = 0; i < numBoneCombination; ++i) boneCombination.push_back(bc[i]);
	for (unsigned long i = 0; i < numBoneCombination; ++i) {
		boneCombination[i].BoneId = new unsigned long[palleteSize];
		memcpy(boneCombination[i].BoneId, bc[i].BoneId, palleteSize*sizeof(unsigned long));	 // ディープコピー
	}
	delete[] pAdjacencyIn;
	pBoneCombinationBuffer->Release();
}

void SkinMesh::CreateShader() {
	CGeneralFactory<CSkinMeshEffect>::Instance().Create(CEffect::eSKIN_MESH, 0, m_effect);
}

vector<D3DXMATRIX> SkinMesh::GetWorlds(const D3DXMATRIX* world) {
	class WorldsCalc {	// ワールド変換行列の配列を計算する再帰関数を定義したクラス
	public:
		static void Run(Bone* me, const D3DXMATRIX *parentWorldMat, vector<D3DXMATRIX>* worlds) {
			D3DXMATRIX boneMat = me->boneMat*(*parentWorldMat);
			(*worlds)[me->id] = me->offsetMat * boneMat;
			if (me->firstChild) Run(me->firstChild, &boneMat, worlds);
			if (me->sibling)	Run(me->sibling, parentWorldMat, worlds);
		}
	};
	vector<D3DXMATRIX> worlds(bones.size());
	WorldsCalc::Run(&bones[0], world, &worlds);
	return worlds;
}

void SkinMesh::Draw(D3DXMATRIX world, D3DLIGHT9* light, D3DXMATRIX view, D3DXMATRIX proj) {
	UpdateBoneMatrix();
	vector<D3DXMATRIX> worlds = GetWorlds(&world);
	D3DXMATRIX spCamera;

	////// プログラマブルシェーダ設定

	// 描画命令を作成、pushする。
	for (unsigned long i = 0; i < boneCombination.size(); ++i) {
		unsigned long numBlendMatrix = 0;
		for (unsigned long k = 0; k < palleteSize; ++k) if (boneCombination[i].BoneId[k] != UINT_MAX) numBlendMatrix = k + 1;
		vector<D3DXMATRIX> selectedCombMat;
		selectedCombMat.reserve(numBlendMatrix);
		for (unsigned long k = 0; k < numBlendMatrix; ++k){
			selectedCombMat.push_back(worlds[boneCombination[i].BoneId[k]]);
		}
		sp<CGraphicsMeshBone> spOrder( new CGraphicsMeshBone() );
		spOrder->SetMesh(
			&pMesh,
			&materials[boneCombination[i].AttribId],
			textures[boneCombination[i].AttribId],
			selectedCombMat,
			m_effect,
			view,
			proj,
			spCamera,
			i );
		spOrder->Draw(pDevice);
	}
}


void SkinMesh::CreateBoneObj() {
	boneObj.resize(bones.size());
	for (unsigned int i = 0; i < bones.size(); i++) D3DXCreateSphere(pDevice, 0.02f, 4, 2, &boneObj[i], 0);
}

void SkinMesh::DrawBoneObj(D3DXVECTOR3* position, D3DXMATRIX* rotation, D3DLIGHT9* light ) {
	UpdateBoneMatrix();
	D3DMATERIAL9 material = {{0.0f, 0.0f, 1.0f, 1.0f}};
	D3DXMATRIX trans, world;
	D3DXMatrixTranslation(&trans, position->x, position->y, position->z);
	world = (*rotation)*trans;
	vector<D3DXMATRIX> worlds = GetWorlds(&world);
	D3DXMATRIX view, projection;
	pDevice->SetLight(0, light);
	pDevice->LightEnable(0, TRUE) ;
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetMaterial(&material);
	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDevice->SetTransform(D3DTS_VIEW, &view);
	pDevice->SetTransform(D3DTS_PROJECTION, &projection);
	for (unsigned int i = 0; i < bones.size(); ++i) {
		// 表示用ボーンのメッシュはもともと原点にあるため、ボーンオフセット行列をかける必要がない
		// ワールド変換行列に逆行列をかけてボーンオフセット行列をキャンセルする
		D3DXMATRIX offsetCanceller;		
		D3DXMatrixInverse(&offsetCanceller, 0, &bones[i].offsetMat);
		pDevice->SetTransform(D3DTS_WORLD, &(offsetCanceller*worlds[i]));
		if (bones[i].type != 7) boneObj[i]->DrawSubset(0);
	}
	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}






///// MmdSkinMeshクラス /////
MmdSkinMesh::MmdSkinMesh(LPCTSTR pmdFilename, LPCTSTR vmdFilename, LPDIRECT3DDEVICE9 pDev) : SkinMesh(pDev) {
	vector<MmdStruct::PmdVertex> pmdVertices;
	vector<unsigned short> pmdFaces;
	vector<MmdStruct::PmdMaterial> pmdMaterials;
	vector<MmdStruct::PmdBone> pmdBones;
	LoadPmdfile(pmdFilename, pmdVertices, pmdFaces, pmdMaterials, pmdBones);
	CreateMesh(pmdVertices, pmdFaces, pmdMaterials, pmdBones.size());
	CreateBoneMatrix(pmdBones);
	CreateBoneObj();
	CreateShader();
	vmdMotionController = new VmdMotionController(vmdFilename, &bones, &pmdIkData);
}

MmdSkinMesh::~MmdSkinMesh() {
	SAFE_DELETE(vmdMotionController);
}

void MmdSkinMesh::LoadPmdfile(const LPCTSTR& filename, vector<MmdStruct::PmdVertex>& pmdVertices, vector<unsigned short>& pmdFaces,
								 vector<MmdStruct::PmdMaterial>& pmdMaterials, vector<MmdStruct::PmdBone>& pmdBones) {
    ifstream ifs(filename, ios::binary);
	if (ifs.fail()) throw TEXT("ファイルがありません");
	MmdStruct::PmdHeader pmdHeader;
	ifs.read((char*)&pmdHeader, sizeof(pmdHeader));
	unsigned long numPmdVertex;
	ifs.read((char*)&numPmdVertex, sizeof(numPmdVertex));
	pmdVertices.resize(numPmdVertex);
	ifs.read((char*)&pmdVertices[0], sizeof(MmdStruct::PmdVertex)*numPmdVertex);
	unsigned long numPmdFace;
	ifs.read((char*)&numPmdFace, sizeof(numPmdFace));
	pmdFaces.resize(numPmdFace);
	ifs.read((char*)&pmdFaces[0], sizeof(unsigned short)*numPmdFace);
	unsigned long numPmdMaterial;
	ifs.read((char*)&numPmdMaterial, sizeof(numPmdMaterial));
	pmdMaterials.resize(numPmdMaterial);
	ifs.read((char*)&pmdMaterials[0], sizeof(MmdStruct::PmdMaterial)*numPmdMaterial);
	unsigned short numPmdBone;
	ifs.read((char*)&numPmdBone, sizeof(numPmdBone));
	pmdBones.resize(numPmdBone);
	ifs.read((char*)&pmdBones[0], sizeof(MmdStruct::PmdBone)*numPmdBone);
	unsigned short numPmdIkData;
	ifs.read((char*)&numPmdIkData, sizeof(numPmdIkData));
	pmdIkData.resize(numPmdIkData);
	for (unsigned short i = 0; i < numPmdIkData; ++i) {
		MmdStruct::PmdIkDataWithoutArray pmdIkDataWithoutArray;
		ifs.read((char*)&pmdIkDataWithoutArray, sizeof(MmdStruct::PmdIkDataWithoutArray));
		vector<unsigned short> ik_child_bone_index(pmdIkDataWithoutArray.ik_chain_length);
		ifs.read((char*)&ik_child_bone_index[0], sizeof(unsigned short)*pmdIkDataWithoutArray.ik_chain_length);
		pmdIkData[i].ik_bone_index = pmdIkDataWithoutArray.ik_bone_index;
		pmdIkData[i].ik_target_bone_index = pmdIkDataWithoutArray.ik_target_bone_index;
		pmdIkData[i].ik_chain_length = pmdIkDataWithoutArray.ik_chain_length;
		pmdIkData[i].iterations = pmdIkDataWithoutArray.iterations;
		pmdIkData[i].control_weight = pmdIkDataWithoutArray.control_weight;
		pmdIkData[i].ik_child_bone_index = ik_child_bone_index;
	}
}

void MmdSkinMesh::CopyMaterial(D3DMATERIAL9& material, const MmdStruct::PmdMaterial& pmdMaterial) {
	const float damp = 0.8f;	// アンビエントを弱くする係数
	material.Ambient.a = pmdMaterial.alpha;
	material.Ambient.r = damp*pmdMaterial.mirror_color[0];
	material.Ambient.g = damp*pmdMaterial.mirror_color[1];
	material.Ambient.b = damp*pmdMaterial.mirror_color[2];
	material.Diffuse.a = pmdMaterial.alpha;
	material.Diffuse.r = pmdMaterial.diffuse_color[0];
	material.Diffuse.g = pmdMaterial.diffuse_color[1];
	material.Diffuse.b = pmdMaterial.diffuse_color[2];
	material.Power = pmdMaterial.specularity;
	material.Specular.a = pmdMaterial.alpha;
	material.Specular.r = pmdMaterial.specular_color[0];
	material.Specular.g = pmdMaterial.specular_color[1];
	material.Specular.b = pmdMaterial.specular_color[2];
}


void MmdSkinMesh::CreateMesh(vector<MmdStruct::PmdVertex> pmdVertices, vector<unsigned short> pmdFaces, vector<MmdStruct::PmdMaterial> pmdMaterials, const unsigned int numBone) {
	// メッシュの入れ物を作成
	LPD3DXMESH pOrgMesh;
	D3DVERTEXELEMENT9 declAry[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
		{0, 24, D3DDECLTYPE_USHORT4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
		{0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 44, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	unsigned int numFace = pmdFaces.size() / 3;
	unsigned int numVertex = pmdVertices.size();
	D3DXCreateMesh(numFace, numVertex, D3DXMESH_MANAGED, declAry, pDevice, &pOrgMesh);
	// 頂点データ収納
	vector<BlendVertex> blendVertices;
	BlendVertex* vertexBuffer;
	pOrgMesh->LockVertexBuffer(0, (void**)&vertexBuffer);
	for (unsigned int i = 0; i < pmdVertices.size(); ++i) {
		BlendVertex bv;
		bv.position = MmdStruct::scale*D3DXVECTOR3(pmdVertices[i].pos[0], pmdVertices[i].pos[1], pmdVertices[i].pos[2]); // DirectXのスケールに合わせる!
		bv.normal = D3DXVECTOR3(pmdVertices[i].normal_vec[0], pmdVertices[i].normal_vec[1], pmdVertices[i].normal_vec[2]);
		bv.texture = D3DXVECTOR2(pmdVertices[i].uv[0], pmdVertices[i].uv[1]);
		bv.weight[0] = (float)pmdVertices[i].bone_weight/100.0f;
		bv.weight[1] = 1.0f - bv.weight[0];
		bv.weight[2] = 0;		// メッシュ分割に必須
		bv.matrixIndex[0] = (unsigned char)pmdVertices[i].bone_num[0];
		bv.matrixIndex[1] = (unsigned char)pmdVertices[i].bone_num[1];
		bv.matrixIndex[2] = 0;	// メッシュ分割に必須
		bv.matrixIndex[3] = 0;	// メッシュ分割に必須
		vertexBuffer[i] = bv;
		blendVertices.push_back(bv);
	}
	pOrgMesh->UnlockVertexBuffer();
	// ポリゴンデータ収納
	unsigned short* indexBuffer;
	pOrgMesh->LockIndexBuffer(0, (void**)&indexBuffer);
	for (unsigned int i = 0; i < pmdFaces.size(); ++i) indexBuffer[i] = pmdFaces[i];
	pOrgMesh->UnlockIndexBuffer();
	// 材料データ格納
	vector<unsigned long> material_number(numFace);	// ポリゴン-材料番号対応配列
	unsigned int j = 0, material_end = 0;
	for (unsigned int i = 0; i < pmdMaterials.size(); ++i) {
		D3DMATERIAL9 material = {0};
		CopyMaterial(material, pmdMaterials[i]);
		material_end += pmdMaterials[i].face_vert_count;
		for (; j < material_end; ++j) material_number[j / 3] = i;
		materials.push_back(material);
		// テクスチャ
		char tex[21] = {0};	// ファイル名が20byteのときのために最後に0を追加
		memcpy(tex, pmdMaterials[i].texture_file_name, 20);
		string s(tex);
		s = s.substr(0, s.find("*"));	// temp
		textures.push_back(0);
		char tex_filename[256] = {0};		// UNICODE未対応テクスチャファイル名
		TCHAR textureFilename[256] = {0};	// UNICODE/マルチバイト両対応テクスチャファイル名
		if (strcpy_s(tex_filename, s.c_str())) throw TEXT("テクスチャの読み込みに失敗しました");
#ifdef UNICODE
		if (strlen(tex_filename) > 0) MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, tex_filename, strlen(tex_filename), textureFilename, (sizeof textureFilename)/2);
#else
		if (strlen(tex_filename) > 0) strcpy_s(textureFilename, tex_filename);
#endif
		if (lstrlen(textureFilename) > 0) // UNICODE/マルチバイト両対応テクスチャファイル名からテクスチャを作成
			if(FAILED(D3DXCreateTextureFromFileEx(pDevice, textureFilename, 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, 0, 0, &textures[i]))) throw TEXT("テクスチャの読み込みに失敗しました");
	}
	// 材料番号収納
	unsigned long* attributeBuffer;
	pOrgMesh->LockAttributeBuffer(0, &attributeBuffer);
	for (unsigned int i = 0; i < numFace; ++i) attributeBuffer[i] = material_number[i];
	pOrgMesh->UnlockAttributeBuffer();
	// メッシュを分割
	LPD3DXSKININFO pSkinInfo = CreateSkinInfo(blendVertices, numBone, declAry);
	DivideMesh(numFace, pOrgMesh, pSkinInfo);
	SAFE_RELEASE(pSkinInfo);
	SAFE_RELEASE(pOrgMesh);
}

void MmdSkinMesh::CreateBoneMatrix(vector<MmdStruct::PmdBone> pmdBones) {
	unsigned int size = pmdBones.size();
	bones.resize(size);
	for (unsigned int i = 0; i < size; ++i) {
		int parentIdx(-1), firstChildIdx(-1), siblingIdx(-1);
		if (pmdBones[i].parent_bone_index != 0xFFFF) parentIdx = pmdBones[i].parent_bone_index;
		for (unsigned int j = i + 1; j < size; ++j) {
			if (pmdBones[i].parent_bone_index == pmdBones[j].parent_bone_index) {
				siblingIdx = j;
				break;
			}
		}
		for (unsigned int j = 0; j < size; ++j) {
			if (i == pmdBones[j].parent_bone_index) {
				firstChildIdx = j;
				break;
			}
		}
		D3DXMATRIX modelLocalInitMat;
		D3DXMatrixTranslation(&modelLocalInitMat, MmdStruct::scale*pmdBones[i].bone_head_pos[0], MmdStruct::scale*pmdBones[i].bone_head_pos[1], MmdStruct::scale*pmdBones[i].bone_head_pos[2]);
		char boneName[21] = {0};	// ボーン名が20byteのときのために最後に0を追加
		memcpy(boneName, pmdBones[i].bone_name, 20);
		bones[i].id = i;
		if (parentIdx != -1) bones[i].parent = &bones[parentIdx];
		if (firstChildIdx != -1) bones[i].firstChild = &bones[firstChildIdx];
		if (siblingIdx != -1) bones[i].sibling = &bones[siblingIdx];
		bones[i].name = boneName;
		bones[i].type = pmdBones[i].bone_type;
		bones[i].initMat = modelLocalInitMat;	// モデルローカル座標系
		D3DXMatrixInverse(&bones[i].offsetMat, 0, &bones[i].initMat);
	}
	class InitMatCalc {	// InitMatをボーンローカル座標系に変換する再帰関数を定義したクラス
	public:
		static void Run(Bone* me, D3DXMATRIX *parentoffsetMat) {
			if (me->firstChild) Run(me->firstChild, &me->offsetMat);
			if (me->sibling)	Run(me->sibling, parentoffsetMat);
			if (parentoffsetMat) me->initMat *= *parentoffsetMat;
		}
	};
	InitMatCalc::Run(&bones[0], 0);	// ボーンローカル座標系に変換
	for (unsigned int i = 0; i < size; ++i) bones[i].boneMat = bones[i].initMat;
}

void MmdSkinMesh::UpdateBoneMatrix() { vmdMotionController->UpdateBoneMatrix(); }

void MmdSkinMesh::AdvanceTime() { vmdMotionController->AdvanceTime(); }




///// PmxSkinMeshクラス /////
PmxSkinMesh::PmxSkinMesh( LPDIRECT3DDEVICE9 pDev) : SkinMesh(pDev) {
}

PmxSkinMesh::~PmxSkinMesh() {
	//SAFE_DELETE(vmdMotionController);
}

bool PmxSkinMesh::LoadModel(LPCTSTR pmxFilename, LPDIRECT3DDEVICE9 pDevice)
{
	CLoadPMX loadPmx;
	string path = CPathControll::Instance().GetAlternatePath(pmxFilename);
	loadPmx.LoadPMXFile(pmxFilename);
	CreateMesh(*loadPmx.GetPmxData(), path);
	CreateBoneMatrix(*loadPmx.GetPmxData());
	CreateIkData(pmxIkData, *loadPmx.GetPmxData());
	CreateBoneObj();
	CreateShader();
	return true;
}
void PmxSkinMesh::CopyMaterial(D3DMATERIAL9& material, const PmxStruct::PMX_MATERIAL& pmxMaterial) {
	const float damp = 0.8f;	// アンビエントを弱くする係数
	material.Ambient.a = pmxMaterial.Diffuse[3];
	material.Ambient.r = damp*pmxMaterial.Ambient[0];
	material.Ambient.g = damp*pmxMaterial.Ambient[1];
	material.Ambient.b = damp*pmxMaterial.Ambient[2];
	material.Diffuse.a = pmxMaterial.Diffuse[3];
	material.Diffuse.r = pmxMaterial.Diffuse[0];
	material.Diffuse.g = pmxMaterial.Diffuse[1];
	material.Diffuse.b = pmxMaterial.Diffuse[2];
	material.Power = pmxMaterial.SpecularPower;
	material.Specular.a = pmxMaterial.Diffuse[3];
	material.Specular.r = pmxMaterial.Specular[0];
	material.Specular.g = pmxMaterial.Specular[1];
	material.Specular.b = pmxMaterial.Specular[2];
}


void PmxSkinMesh::CreateIkData(vector<MmdStruct::PmdIkData>& pmdIkData, PmxStruct::PMX_DATA& readData )
{
	int iknum=0;
	pmdIkData.resize(readData.s_PmxIKNum);
	for(unsigned short i = 0; i < readData.s_PmxBoneNum; ++i ){
		if( readData.s_pPmxBone[i].Flag_IK ){
			MmdStruct::PmdIkDataWithoutArray pmdIkDataWithoutArray;
			pmdIkDataWithoutArray.ik_bone_index=i;
			pmdIkDataWithoutArray.ik_target_bone_index = readData.s_pPmxBone[i].IKInfo.TargetBoneIndex;
			pmdIkDataWithoutArray.control_weight = readData.s_pPmxBone[i].IKInfo.RotLimit;
			pmdIkDataWithoutArray.iterations = readData.s_pPmxBone[i].IKInfo.LoopNum;
			pmdIkDataWithoutArray.ik_chain_length = readData.s_pPmxBone[i].IKInfo.LinkNum;
			vector<unsigned short> ik_child_bone_index(pmdIkDataWithoutArray.ik_chain_length);
			for(unsigned char j = 0; j < pmdIkDataWithoutArray.ik_chain_length; ++j )
				ik_child_bone_index[j]=readData.s_pPmxBone[i].IKInfo.Link[j].BoneIndex;
			pmdIkData[iknum].ik_bone_index = pmdIkDataWithoutArray.ik_bone_index;
			pmdIkData[iknum].ik_target_bone_index = pmdIkDataWithoutArray.ik_target_bone_index;
			pmdIkData[iknum].ik_chain_length = pmdIkDataWithoutArray.ik_chain_length;
			pmdIkData[iknum].iterations = pmdIkDataWithoutArray.iterations;
			pmdIkData[iknum].control_weight = pmdIkDataWithoutArray.control_weight;
			pmdIkData[iknum].ik_child_bone_index = ik_child_bone_index;
			iknum++;
		}
	}
}
void PmxSkinMesh::CreateMesh(PmxStruct::PMX_DATA& readData, string _src){
	// メッシュの入れ物を作成
	LPD3DXMESH pOrgMesh;
	D3DVERTEXELEMENT9 declAry[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
		{0, 24, D3DDECLTYPE_USHORT4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
		{0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 44, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	unsigned int numFace = readData.s_PmxFaceNum;
	unsigned int numVertex = readData.s_PmxVertexNum;

	HRESULT hr = D3DXCreateMesh(numFace, numVertex, D3DXMESH_MANAGED, declAry, pDevice, &pOrgMesh);
	if (FAILED(hr)){
		const char *buf;
		buf = DXGetErrorString(hr);
		char str[128];
		sprintf(str, "ポリゴン数　%d\0", numFace);
		MessageBox(NULL, str,buf, MB_OK);
		exit(0);
	}

	// 頂点データ収納
	vector<BlendVertex> blendVertices;
	vector<PmxStruct::PMX_VERTEX> pmxvertex;
	BlendVertex* vertexBuffer;
	pOrgMesh->LockVertexBuffer(0, (void**)&vertexBuffer);

	for (unsigned int i = 0; i < readData.s_PmxVertexNum; ++i) {
		BlendVertex bv;
		bv.position = scale*D3DXVECTOR3(readData.s_pPmxVertex[i].Position[0], readData.s_pPmxVertex[i].Position[1], readData.s_pPmxVertex[i].Position[2]); // DirectXのスケールに合わせる!
		bv.normal = D3DXVECTOR3(readData.s_pPmxVertex[i].Normal[0], readData.s_pPmxVertex[i].Normal[1], readData.s_pPmxVertex[i].Normal[2]);
		bv.texture = D3DXVECTOR2(readData.s_pPmxVertex[i].UV[0], readData.s_pPmxVertex[i].UV[1]);
		bv.weight[0] = readData.s_pPmxVertex[i].BoneWeight[0];
		bv.weight[1] = 1.0f - bv.weight[0];
		bv.weight[2] = 0;		// メッシュ分割に必須
		bv.matrixIndex[0] = (unsigned char)readData.s_pPmxVertex[i].BoneIndex[0];
		if( readData.s_pPmxVertex[i].BoneIndex[1] == -1 )
			bv.matrixIndex[1] = (unsigned char)readData.s_pPmxVertex[i].BoneIndex[0]+1;	// メッシュ分割に必須
		else
			bv.matrixIndex[1] = (unsigned char)readData.s_pPmxVertex[i].BoneIndex[1];
		bv.matrixIndex[2] = 0;	// メッシュ分割に必須
		bv.matrixIndex[3] = 0;	// メッシュ分割に必須
		vertexBuffer[i] = bv;
		blendVertices.push_back(bv);
		pmxvertex.push_back(readData.s_pPmxVertex[i]);
	}
	pOrgMesh->UnlockVertexBuffer();
	// ポリゴンデータ収納
	unsigned short* indexBuffer;
	pOrgMesh->LockIndexBuffer(0, (void**)&indexBuffer);
	for (unsigned int i = 0; i < readData.s_PmxFaceNum; ++i){
		indexBuffer[i*3] = readData.s_pPmxFace[i].VertexIndex[0];
		indexBuffer[(i*3)+1] = readData.s_pPmxFace[i].VertexIndex[1];
		indexBuffer[(i*3)+2] = readData.s_pPmxFace[i].VertexIndex[2];
	}
	pOrgMesh->UnlockIndexBuffer();
	// 材料データ格納
	vector<unsigned long> material_number(numFace);	// ポリゴン-材料番号対応配列
	unsigned int j = 0, material_end = 0;
	for (unsigned int i = 0; i < readData.s_PmxMaterialNum; ++i) {
		D3DMATERIAL9 material = { 0 };
		CopyMaterial(material, readData.s_pPmxMaterial[i]);
		material_end += readData.s_pPmxMaterial[i].MaterialFaceNum;
		for (; j < material_end; ++j) material_number[j / 3] = i;
		materials.push_back(material);
		// テクスチャ
		char tex[21] = { 0 };	// ファイル名が20byteのときのために最後に0を追加
		memcpy(tex,
			readData.s_pPmxTexture[readData.s_pPmxMaterial[i].TextureIndex].TexturePath, 20);
		string s(tex);
		s = s.substr(0, s.find("*"));	// temp
		s = _src + s;
		textures.push_back(0);
		char tex_filename[256] = { 0 };		// UNICODE未対応テクスチャファイル名
		TCHAR textureFilename[256] = { 0 };	// UNICODE/マルチバイト両対応テクスチャファイル名
		if (strcpy_s(tex_filename, s.c_str())) throw TEXT("テクスチャの読み込みに失敗しました");
#ifdef UNICODE
		if (strlen(tex_filename) > 0) MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, tex_filename, strlen(tex_filename), textureFilename, (sizeof textureFilename)/2);
#else
		if (strlen(tex_filename) > 0) strcpy_s(textureFilename, tex_filename);
#endif
		if (lstrlen(textureFilename) > 0) {// UNICODE/マルチバイト両対応テクスチャファイル名からテクスチャを作成
			HRESULT hr = D3DXCreateTextureFromFileEx(pDevice, textureFilename, 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, 0, 0, &textures[i]);
			if (FAILED(hr)){
				string str(textureFilename);
				::MessageBox(NULL, str.c_str(), "テクスチャの読み込みに失敗しました", MB_OK);
			}
		}
	}
	// 材料番号収納
	unsigned long* attributeBuffer;
	pOrgMesh->LockAttributeBuffer(0, &attributeBuffer);
	for (unsigned int i = 0; i < numFace; ++i) attributeBuffer[i] = material_number[i];
	pOrgMesh->UnlockAttributeBuffer();

	// メッシュを分割
	LPD3DXSKININFO pSkinInfo = CreateSkinInfo(blendVertices, readData.s_PmxBoneNum, declAry);
	DivideMesh(numFace, pOrgMesh, pSkinInfo);
	SAFE_RELEASE(pSkinInfo);
	SAFE_RELEASE(pOrgMesh);
}

void PmxSkinMesh::CreateBoneMatrix( PmxStruct::PMX_DATA& readData ) {
	unsigned int size = readData.s_PmxBoneNum;
	bones.resize(size);
	for (unsigned int i = 0; i < size; ++i) {
		int parentIdx(-1), firstChildIdx(-1), siblingIdx(-1);
		if (readData.s_pPmxBone[i].ParentBoneIndex != 0xFFFF) parentIdx = readData.s_pPmxBone[i].ParentBoneIndex;
		for (unsigned int j = i + 1; j < size; ++j) {
			if (readData.s_pPmxBone[i].ParentBoneIndex == readData.s_pPmxBone[j].ParentBoneIndex) {
				siblingIdx = j;
				break;
			}
		}
		for (unsigned int j = 0; j < size; ++j) {
			if (i ==readData.s_pPmxBone[j].ParentBoneIndex) {
				firstChildIdx = j;
				break;
			}
		}
		D3DXMATRIX modelLocalInitMat;
		D3DXMatrixTranslation(&modelLocalInitMat, MmdStruct::scale*readData.s_pPmxBone[i].Position[0], MmdStruct::scale*readData.s_pPmxBone[i].Position[1], MmdStruct::scale*readData.s_pPmxBone[i].Position[2]);
		char boneName[21] = {0};	// ボーン名が20byteのときのために最後に0を追加
		memcpy(boneName, readData.s_pPmxBone[i].Name, 20);
		bones[i].id = i;
		if (parentIdx != -1) bones[i].parent = &bones[parentIdx];
		if (firstChildIdx != -1) bones[i].firstChild = &bones[firstChildIdx];
		if (siblingIdx != -1) bones[i].sibling = &bones[siblingIdx];
		bones[i].name = boneName;
		bones[i].type = 0;
		if(readData.s_pPmxBone[i].Flag_IK)
			bones[i].type = 7;
		bones[i].initMat = modelLocalInitMat;	// モデルローカル座標系
		D3DXMatrixInverse(&bones[i].offsetMat, 0, &bones[i].initMat);
	}
	class InitMatCalc {	// InitMatをボーンローカル座標系に変換する再帰関数を定義したクラス
	public:
		static void Run(Bone* me, D3DXMATRIX *parentoffsetMat) {
			if (me->firstChild) Run(me->firstChild, &me->offsetMat);
			if (me->sibling)	Run(me->sibling, parentoffsetMat);
			if (parentoffsetMat) me->initMat *= *parentoffsetMat;
		}
	};
	InitMatCalc::Run(&bones[0], 0);	// ボーンローカル座標系に変換
	for (unsigned int i = 0; i < size; ++i) bones[i].boneMat = bones[i].initMat;
}

void PmxSkinMesh::UpdateBoneMatrix() { m_spVmdMotionController->UpdateBoneMatrix(); }

void PmxSkinMesh::AdvanceTime() { m_spVmdMotionController->AdvanceTime(); }
