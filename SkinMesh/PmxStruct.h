#pragma once

#include <windows.h>
#include <vector>

using namespace std;

// �}�N����` -----------------------------------

#define PMX_MAX_IKLINKNUM		(64)			// �Ή�����IK�����N�̍ő吔

// FLOAT4�f�[�^�^
typedef struct tagFLOAT4
{
	float					x, y, z, w ;
} FLOAT4, *LPFLOAT4 ;

/// PMX��p�\���̒�`
namespace PmxStruct {
#pragma pack(push,1)	//�A���C�������g������I�t


	// PMX�t�@�C���̏����i�[����\����
	struct PMX_BASEINFO
	{
		BYTE	EncodeType ;						// �����R�[�h�̃G���R�[�h�^�C�v 0:UTF16 1:UTF8
		BYTE	UVNum ;								// �ǉ��t�u�� ( 0�`4 )
		BYTE	VertexIndexSize ;					// ���_Index�T�C�Y ( 1 or 2 or 4 )
		BYTE	TextureIndexSize ;					// �e�N�X�`��Index�T�C�Y ( 1 or 2 or 4 )
		BYTE	MaterialIndexSize ;					// �}�e���A��Index�T�C�Y ( 1 or 2 or 4 )
		BYTE	BoneIndexSize ;						// �{�[��Index�T�C�Y ( 1 or 2 or 4 )
		BYTE	MorphIndexSize ;					// ���[�tIndex�T�C�Y ( 1 or 2 or 4 )
		BYTE	RigidIndexSize ;					// ����Index�T�C�Y ( 1 or 2 or 4 )
	};
	
	// ���_�f�[�^
	struct PMX_VERTEX
	{
		float	Position[ 3 ] ;						// ���W
		float	Normal[ 3 ] ;						// �@��
		float	UV[ 2 ] ;							// �W��UV�l
		float	AddUV[ 4 ][ 4 ] ;					// �ǉ�UV�l
		BYTE	WeightType ;						// �E�G�C�g�^�C�v( 0:BDEF1 1:BDEF2 2:BDEF4 3:SDEF )
		int		BoneIndex[ 4 ] ;					// �{�[���C���f�b�N�X
		float	BoneWeight[ 4 ] ;					// �{�[���E�F�C�g
		float	SDEF_C[ 3 ] ;						// SDEF-C
		float	SDEF_R0[ 3 ] ;						// SDEF-R0
		float	SDEF_R1[ 3 ] ;						// SDEF-R1
		float	ToonEdgeScale ;						// �g�D�[���G�b�W�̃X�P�[��
	};

	// �ʃ��X�g
	struct PMX_FACE
	{
		int		VertexIndex[ 3 ] ;
	} ;

	// �e�N�X�`�����
	struct PMX_TEXTURE
	{
		char	TexturePath[ 512 ] ;
	} ;

	// �}�e���A�����
	struct PMX_MATERIAL
	{
		char	Name[ 128 ] ;						// ���O

		float	Diffuse[ 4 ] ;						// �f�B�t���[�Y�J���[
		float	Specular[ 3 ] ;						// �X�y�L�����J���[
		float	SpecularPower ;						// �X�y�L�����萔
		float	Ambient[ 3 ] ;						// �A���r�G���g�J���[

		BYTE	CullingOff ;						// ���ʕ`��
		BYTE	GroundShadow ;						// �n�ʉe
		BYTE	SelfShadowMap ;						// �Z���t�V���h�E�}�b�v�ւ̕`��
		BYTE	SelfShadowDraw ;					// �Z���t�V���h�E�̕`��
		BYTE	EdgeDraw ;							// �G�b�W�̕`��

		float	EdgeColor[ 4 ] ;					// �G�b�W�J���[
		float	EdgeSize ;							// �G�b�W�T�C�Y

		int		TextureIndex ;						// �ʏ�e�N�X�`���C���f�b�N�X
		int		SphereTextureIndex ;				// �X�t�B�A�e�N�X�`���C���f�b�N�X
		BYTE	SphereMode ;						// �X�t�B�A���[�h( 0:���� 1:��Z 2:���Z 3:�T�u�e�N�X�`��(�ǉ�UV1��x,y��UV�Q�Ƃ��Ēʏ�e�N�X�`���`����s��)

		BYTE	ToonFlag ;							// ���L�g�D�[���t���O( 0:��Toon 1:���LToon )
		int		ToonTextureIndex ;					// �g�D�[���e�N�X�`���C���f�b�N�X

		int		MaterialFaceNum ;					// �}�e���A�����K������Ă���ʂ̐�
	} ;


	// �h�j�����N���
	struct PMX_IKLINK
	{
		int		BoneIndex ;							// �����N�{�[���̃C���f�b�N�X
		BYTE	RotLockFlag ;						// ��]����( 0:OFF 1:ON )
		float	RotLockMin[ 3 ] ;					// ��]�����A����
		float	RotLockMax[ 3 ] ;					// ��]�����A���
	} ;

	// �h�j���
	struct PMX_IK
	{
		int		TargetBoneIndex ;					// IK�^�[�Q�b�g�̃{�[���C���f�b�N�X
		int		LoopNum ;							// IK�v�Z�̃��[�v��
		float	RotLimit ;							// �v�Z���ӂ�̐����p�x

		int		LinkNum ;							// �h�j�����N�̐�
		PMX_IKLINK Link[ PMX_MAX_IKLINKNUM ] ;		// �h�j�����N���
	} ;

	// �{�[�����
	struct PMX_BONE
	{
		char	Name[ 128 ] ;						// ���O
		float	Position[ 3 ] ;						// ���W
		int		ParentBoneIndex ;					// �e�{�[���C���f�b�N�X
		int		TransformLayer ;					// �ό`�K�w
		
		BYTE	Flag_LinkDest ;						// �ڑ���
		BYTE	Flag_EnableRot ;					// ��]���ł��邩
		BYTE	Flag_EnableMov ;					// �ړ����ł��邩
		BYTE	Flag_Disp ;							// �\��
		BYTE	Flag_EnableControl ;				// ���삪�ł��邩
		BYTE	Flag_IK ;							// IK
		BYTE	Flag_AddRot ;						// ��]�t�^
		BYTE	Flag_AddMov ;						// �ړ��t�^
		BYTE	Flag_LockAxis ;						// ���Œ�
		BYTE	Flag_LocalAxis ;					// ���[�J����
		BYTE	Flag_AfterPhysicsTransform ;		// ������ό`
		BYTE	Flag_OutParentTransform ;			// �O���e�ό`

		float	OffsetPosition[ 3 ] ;				// �I�t�Z�b�g���W
		int		LinkBoneIndex ;						// �ڑ���{�[���C���f�b�N�X
		int		AddParentBoneIndex ;				// �t�^�e�{�[���C���f�b�N�X
		float	AddRatio ;							// �t�^��
		float	LockAxisVector[ 3 ] ;				// ���Œ莞�̎��̕����x�N�g��
		float	LocalAxisXVector[ 3 ] ;				// ���[�J�����̂w��
		float	LocalAxisZVector[ 3 ] ;				// ���[�J�����̂y��
		int		OutParentTransformKey ;				// �O���e�ό`���� Key�l

		PMX_IK	IKInfo ;							// �h�j���
	} ;

	// ���_���[�t���
	struct PMX_MORPH_VERTEX
	{
		int		Index ;								// ���_�C���f�b�N�X
		float	Offset[ 3 ] ;						// ���_���W�I�t�Z�b�g
	} ;

	// �t�u���[�t���
	struct PMX_MORPH_UV
	{
		int		Index ;								// ���_�C���f�b�N�X
		FLOAT4	Offset ;							// ���_�t�u�I�t�Z�b�g
	} ;

	// �{�[�����[�t���
	struct PMX_MORPH_BONE
	{
		int		Index ;								// �{�[���C���f�b�N�X
		float	Offset[ 3 ] ;						// ���W�I�t�Z�b�g
		float	Quat[ 4 ] ;							// ��]�N�H�[�^�j�I��
	} ;

	// �ގ����[�t���
	struct PMX_MORPH_MATERIAL
	{
		int		Index ;								// �}�e���A���C���f�b�N�X
		BYTE	CalcType ;							// �v�Z�^�C�v( 0:��Z  1:���Z )
		float	Diffuse[ 4 ] ;						// �f�B�t���[�Y�J���[
		float	Specular[ 3 ] ;						// �X�y�L�����J���[
		float	SpecularPower ;						// �X�y�L�����W��
		float	Ambient[ 3 ] ;						// �A���r�G���g�J���[
		float	EdgeColor[ 4 ] ;					// �G�b�W�J���[
		float	EdgeSize ;							// �G�b�W�T�C�Y
		float	TextureScale[ 4 ] ;					// �e�N�X�`���W��
		float	SphereTextureScale[ 4 ] ;			// �X�t�B�A�e�N�X�`���W��
		float	ToonTextureScale[ 4 ] ;				// �g�D�[���e�N�X�`���W��
	} ;

	// �O���[�v���[�t
	struct PMX_MORPH_GROUP
	{
		int		Index ;								// ���[�t�C���f�b�N�X
		float	Ratio ;								// ���[�t��
	} ;

	// ���[�t���
	struct PMX_MORPH
	{
		char	Name[ 128 ] ;						// ���O

		BYTE	ControlPanel ;						// ����p�l��
		BYTE	Type ;								// ���[�t�̎��  0:�O���[�v 1:���_ 2:�{�[�� 3:UV 4:�ǉ�UV1 5:�ǉ�UV2 6:�ǉ�UV3 7:�ǉ�UV4 8:�ގ�

		int		DataNum ;							// ���[�t���̐�

		union
		{
			PMX_MORPH_VERTEX *Vertex ;				// ���_���[�t
			PMX_MORPH_UV *UV ;						// UV���[�t
			PMX_MORPH_BONE *Bone ;					// �{�[�����[�t
			PMX_MORPH_MATERIAL *Material ;			// �}�e���A�����[�t
			PMX_MORPH_GROUP *Group ;				// �O���[�v���[�t
		} ;
	} ;

	// ���̏��
	struct PMX_RIGIDBODY
	{
		char	Name[ 128 ] ;						// ���O

		int		BoneIndex ;							// �Ώۃ{�[���ԍ�

		BYTE	RigidBodyGroupIndex ;				// ���̃O���[�v�ԍ�
		WORD	RigidBodyGroupTarget ;				// ���̃O���[�v�Ώ�

		BYTE	ShapeType ;							// �`��( 0:��  1:��  2:�J�v�Z�� )
		float	ShapeW ;							// ��
		float	ShapeH ;							// ����
		float	ShapeD ;							// ���s

		float	Position[ 3 ] ;						// �ʒu
		float	Rotation[ 3 ] ;						// ��]( ���W�A�� )

		float	RigidBodyWeight ;					// ����
		float	RigidBodyPosDim ;					// �ړ�����
		float	RigidBodyRotDim ;					// ��]����
		float	RigidBodyRecoil ;					// ������
		float	RigidBodyFriction ;					// ���C��

		BYTE	RigidBodyType ;						// ���̃^�C�v( 0:Bone�Ǐ]  1:�������Z  2:�������Z(Bone�ʒu���킹) )
	} ;

	// �W���C���g���
	struct PMX_JOINT
	{
		char	Name[ 128 ] ;						// ���O

		BYTE	Type ;								// ���( 0:�X�v�����O6DOF ( PMX2.0 �ł� 0 �̂� )

		int		RigidBodyAIndex ;					// �ڑ��捄�̂`
		int		RigidBodyBIndex ;					// �ڑ��捄�̂a

		float	Position[ 3 ] ;						// �ʒu
		float	Rotation[ 3 ] ;						// ��]( ���W�A�� )

		float	ConstrainPositionMin[ 3 ] ;			// �ړ�����-����
		float	ConstrainPositionMax[ 3 ] ;			// �ړ�����-���
		float	ConstrainRotationMin[ 3 ] ;			// ��]����-����
		float	ConstrainRotationMax[ 3 ] ;			// ��]����-���

		float	SpringPosition[ 3 ] ;				// �o�l�萔-�ړ�
		float	SpringRotation[ 3 ] ;				// �o�l�萔-��]
	} ;
	
	struct PMX_DATA
	{
		PMX_BASEINFO	s_PmxInfo ;
		DWORD			s_PmxVertexNum;
		DWORD			s_PmxFaceNum ;
		DWORD			s_PmxTextureNum ;
		DWORD			s_PmxMaterialNum;
		DWORD			s_PmxBoneNum;
		DWORD			s_PmxIKNum;
		DWORD			s_PmxMorphNum ;
		DWORD			s_PmxRigidbodyNum ;
		DWORD			s_PmxJointNum ;
		DWORD			s_PmxSkinNum ;
		PMX_VERTEX*		s_pPmxVertex;
		PMX_FACE*		s_pPmxFace ;
		PMX_TEXTURE*	s_pPmxTexture ;
		PMX_MATERIAL*	s_pPmxMaterial;
		PMX_BONE*		s_pPmxBone;
		PMX_MORPH*		s_pPmxMorph ;
		PMX_RIGIDBODY*	s_pPmxRigidbody ;
		PMX_JOINT*		s_pPmxJoint ;
		
	};
		
	/// �X�P�[���ϊ��萔
	const float scale = 0.1f;

#pragma pack(pop)
}