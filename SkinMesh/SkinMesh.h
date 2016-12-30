#pragma once
#include "Interface.h"
#include "VmdMotionController.h"
#include "PmxStruct.h"
#include "CSkinMeshEffect.h"
#include "../DrawOrder/CGraphicsMeshBone.h"

/// �X�L�����b�V��
class SkinMesh : public Drawable3D {
protected:
	/// ��{ ///
	SkinMesh(LPDIRECT3DDEVICE9 pDevice);// �I�u�W�F�N�g�����֎~
	SkinMesh(void);// �I�u�W�F�N�g�����֎~
	LPDIRECT3DDEVICE9 pDevice;			// Direct3D�f�o�C�X�I�u�W�F�N�g
	/// ���b�V�� ///

	LPD3DXMESH pMesh;					// ���b�V��
	vector<D3DMATERIAL9> materials;		// �}�e���A���z��
	vector<LPDIRECT3DTEXTURE9> textures;// �e�N�X�`���z��
	vector<D3DXBONECOMBINATION> boneCombination;// �{�[���R���r�l�[�V�����z��
	virtual LPD3DXSKININFO CreateSkinInfo(const vector<BlendVertex>, const unsigned int, const D3DVERTEXELEMENT9*);	// ���b�V�������̂��߂̃X�L�������쐬
	virtual void DivideMesh(unsigned int numFace, LPD3DXMESH pOrgMesh, LPD3DXSKININFO pSkinInfo);					// �u�����h�s��p���b�g�̃T�C�Y���z���Ȃ��悤�Ƀ��b�V���𕪊�
	/// �{�[�� ///
	vector<Bone> bones;					// �{�[���\����
	vector<Bone>* es;
	vector<ID3DXMesh*> boneObj;			// �\���p�{�[��
	virtual void CreateBoneObj();													// �\���p�{�[�����쐬
	virtual vector<D3DXMATRIX> GetWorlds(const D3DXMATRIX* world);					// �{�[���s�񂩂�ϊ��������[���h�ϊ��s��̔z����擾
	/// �V�F�[�_ ///
	sp<CSkinMeshEffect> m_effect;
	void CreateShader();	// �V�F�[�_�̍쐬
	/// �{�[���s��̍X�V
	virtual void UpdateBoneMatrix() = 0;	// �p����Ɏ���
public:
	virtual ~SkinMesh();
	virtual void AdvanceTime() = 0;						// ���Ԃ������߂�
	vector<Bone>* GetBoneAddress( void ){
		return &bones;
	}
	LPD3DXMESH GetMesh( void ){
		return pMesh;
	}
	virtual void Draw(D3DXMATRIX world, D3DLIGHT9* light, D3DXMATRIX view, D3DXMATRIX proj);		// �`��
	virtual void DrawBoneObj(D3DXVECTOR3* position, D3DXMATRIX* rotation, D3DLIGHT9* light );// �{�[����\��

	vector<Bone> GetBone( void ){
		return bones;
	}

	int GetBoneId(string name){
		for (unsigned int i = 0; i < bones.size(); ++i){
			if (name == bones[i].name)	return bones[i].id;
		}
		if (name.c_str() == NULL){
			printf("�{�[����������܂���\n", name.c_str());
			return NULL;
		}
		else{
			printf("%s��������܂���\n", name.c_str());
			return NULL;
		}
	}
};


/// MMD�X�L�����b�V��
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

// PMX�X�L�����b�V��
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
