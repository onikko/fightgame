//============================================================
//!	@file		CLoadPMX.cpp
//!	@brief		PMX���[�h�N���X
//!	@note		LastEdited //
//!	@author		�n�g�������Q�[���w�ȗL�u
//============================================================

#include "CLoadPMX.h"
#include <iostream>
#include <fstream>

// �e�L�X�g�f�[�^���V�t�gJIS�ɕϊ����Ď擾
__inline void MV1LoadModelToPMX_GetString( BYTE **Src, char *DestBuffer, BYTE EncodeType )
{
	char Buffer[ 1024 ] ;
	wchar_t WBuffer[ 1024 ] ;
	int TextSize ;

	TextSize = *( ( DWORD * )*Src ) ;
	*Src = *Src + 4 ;

	if( EncodeType == 0 )
	{
		// UTF16

		memcpy( WBuffer, *Src, TextSize ) ;
		( ( BYTE * )WBuffer )[ TextSize     ] = 0 ;
		( ( BYTE * )WBuffer )[ TextSize + 1 ] = 0 ;
		*Src = *Src + TextSize ;

		// ���C�h�������V�t�gJIS�ɕϊ�
		WideCharToMultiByte( 932, 0, WBuffer, -1, DestBuffer, 512, NULL, NULL ) ;
	}
	else
	if( EncodeType == 1 )
	{
		// UTF8

		memcpy( Buffer, *Src, TextSize ) ;
		Buffer[ TextSize ] = '\0' ;
		*Src = *Src + TextSize ;

		// �ŏ��� UTF-8 �����C�h�����ɕϊ�
		MultiByteToWideChar( CP_UTF8, 0, Buffer, -1, WBuffer, 1024 ) ;

		// ���C�h�������V�t�gJIS�ɕϊ�
		WideCharToMultiByte( 932, 0, WBuffer, -1, DestBuffer, 512, NULL, NULL ) ;
	}
}


// �����l�T�C�Y�ɍ��킹�������l���擾����(��������)
__inline int MV1LoadModelToPMX_GetInt( BYTE **Src, BYTE Size )
{
	int res = 0 ;

	switch( Size )
	{
	case 1 :
		res = ( char )( ( *Src )[ 0 ] ) ;
		*Src = *Src + 1 ;
		break ;

	case 2 :
		res = ( short )( ( ( WORD * )*Src )[ 0 ] ) ;
		*Src = *Src + 2 ;
		break ;

	case 4 :
		res = ( ( int * )*Src )[ 0 ] ;
		*Src = *Src + 4 ;
		break ;
	}

	return res ;
}

// �����l�T�C�Y�ɍ��킹�������l���擾����(32bit�ȊO�����Ȃ�)
__inline int MV1LoadModelToPMX_GetUInt( BYTE **Src, BYTE Size )
{
	int res = 0 ;

	switch( Size )
	{
	case 1 :
		res = ( int )( ( *Src )[ 0 ] ) ;
		*Src = *Src + 1 ;
		break ;

	case 2 :
		res = ( int )( ( ( WORD * )*Src )[ 0 ] ) ;
		*Src = *Src + 2 ;
		break ;

	case 4 :
		res = ( ( int * )*Src )[ 0 ] ;
		*Src = *Src + 4 ;
		break ;
	}

	return res ;
}

bool CLoadPMX::LoadPMXFile( LPCSTR _src )
{
	//--���[�J���ϐ��錾--
	FILE* fp;
	BYTE* mem;
	BYTE* mem_head;
	BYTE AddHeadDataSize ;
	int i, j;

	if( fopen_s( &fp, _src, "rb ") != 0 ){
		printf("�G���[ : %s���I�[�v���ł��܂���!\n", _src );
		return false;
	}
	fseek(fp, 0, SEEK_END);
	int size = ftell( fp );
	fseek(fp, 0, SEEK_SET );
	mem = ( BYTE * )malloc(sizeof( BYTE ) * size);
	fread(mem, sizeof(char), size, fp );
	fclose( fp );
	mem_head = mem;

	
	// PMX�t�@�C�����ǂ������m�F
	if( mem[ 0 ] != 'P' || mem[ 1 ] != 'M' || mem[ 2 ] != 'X' || mem[ 3 ] != ' ' )
		return false;
	mem += 4 ;

	// Ver2.0 �����m�F
	if( mem[ 0 ] != 0x00 || mem[ 1 ] != 0x00 || mem[ 2 ] != 0x00 || mem[ 3 ] != 0x40 )
	{
		printf( "PMX Load Error : �o�[�W�����Q�D�O�ł͂���܂���\n") ;
		return false ;
	}
	mem += 4 ;
	
	// �ǉ����̃T�C�Y���擾
	AddHeadDataSize = mem[ 0 ] ;
	mem++ ;

	// �ǉ������擾
	m_pmx_data.s_PmxInfo.EncodeType        = mem[ 0 ] ;
	m_pmx_data.s_PmxInfo.UVNum             = mem[ 1 ] ;
	m_pmx_data.s_PmxInfo.VertexIndexSize   = mem[ 2 ] ;
	m_pmx_data.s_PmxInfo.TextureIndexSize  = mem[ 3 ] ;
	m_pmx_data.s_PmxInfo.MaterialIndexSize = mem[ 4 ] ;
	m_pmx_data.s_PmxInfo.BoneIndexSize     = mem[ 5 ] ;
	m_pmx_data.s_PmxInfo.MorphIndexSize    = mem[ 6 ] ;
	m_pmx_data.s_PmxInfo.RigidIndexSize    = mem[ 7 ] ;
	mem += AddHeadDataSize ;
	
	// ���f�����X�L�b�v
	mem += *( ( DWORD * )mem ) + 4 ;
	mem += *( ( DWORD * )mem ) + 4 ;
	mem += *( ( DWORD * )mem ) + 4 ;
	mem += *( ( DWORD * )mem ) + 4 ;

	
	// ���_�����擾
	m_pmx_data.s_PmxVertexNum = *( ( DWORD * )mem ) ;
	mem += 4 ;

	
	m_pmx_data.s_pPmxVertex = ( PMX_VERTEX * )malloc( sizeof( PMX_VERTEX ) * m_pmx_data.s_PmxVertexNum );
	if( m_pmx_data.s_pPmxVertex == NULL )
	{
		printf("PMX Load Error : ���_�f�[�^�̈ꎞ�L���̈�̊m�ۂɎ��s���܂���\n" );
		goto ENDLABEL;
	}
	
	// ���_�f�[�^���擾
	for( i = 0 ; ( DWORD )i < m_pmx_data.s_PmxVertexNum ; i ++ )
	{
		*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].Position[ 0 ] ) = *( ( DWORD * )&mem[ 0 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].Position[ 1 ] ) = *( ( DWORD * )&mem[ 4 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].Position[ 2 ] ) = *( ( DWORD * )&mem[ 8 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].Normal[ 0 ] ) = *( ( DWORD * )&mem[ 12 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].Normal[ 1 ] ) = *( ( DWORD * )&mem[ 16 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].Normal[ 2 ] ) = *( ( DWORD * )&mem[ 20 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].UV[ 0 ] ) = *( ( DWORD * )&mem[ 24 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].UV[ 1 ] ) = *( ( DWORD * )&mem[ 28 ] ) ;

		mem += 32 ;
		for( j = 0 ; j < m_pmx_data.s_PmxInfo.UVNum ; j ++ )
		{
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].AddUV[ j ][ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].AddUV[ j ][ 0 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].AddUV[ j ][ 0 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].AddUV[ j ][ 0 ] ) = *( ( DWORD * )&mem[ 12 ] ) ;
			mem += 16 ;
		}

		m_pmx_data.s_pPmxVertex[ i ].WeightType = mem[ 0 ] ;
		mem ++ ;

		m_pmx_data.s_pPmxVertex[ i ].BoneIndex[ 0 ] = -1 ;
		m_pmx_data.s_pPmxVertex[ i ].BoneIndex[ 1 ] = -1 ;
		m_pmx_data.s_pPmxVertex[ i ].BoneIndex[ 2 ] = -1 ;
		m_pmx_data.s_pPmxVertex[ i ].BoneIndex[ 3 ] = -1 ;
		switch( m_pmx_data.s_pPmxVertex[ i ].WeightType )
		{
		case 0 :	// BDEF1
			m_pmx_data.s_pPmxVertex[ i ].BoneIndex[ 0 ] = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.BoneIndexSize ) ;
			m_pmx_data.s_pPmxVertex[ i ].BoneWeight[ 0 ] = 1.0f ;
			break ;

		case 1 :	// BDEF2
			m_pmx_data.s_pPmxVertex[ i ].BoneIndex[ 0 ] = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.BoneIndexSize ) ;
			m_pmx_data.s_pPmxVertex[ i ].BoneIndex[ 1 ] = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.BoneIndexSize ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].BoneWeight[ 0 ] ) = *( ( DWORD * )mem ) ;
			mem += 4 ;
			m_pmx_data.s_pPmxVertex[ i ].BoneWeight[ 1 ] = 1.0f - m_pmx_data.s_pPmxVertex[ i ].BoneWeight[ 0 ] ;
			break ;

		case 2 :	// BDEF4
			m_pmx_data.s_pPmxVertex[ i ].BoneIndex[ 0 ] = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.BoneIndexSize ) ;
			m_pmx_data.s_pPmxVertex[ i ].BoneIndex[ 1 ] = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.BoneIndexSize ) ;
			m_pmx_data.s_pPmxVertex[ i ].BoneIndex[ 2 ] = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.BoneIndexSize ) ;
			m_pmx_data.s_pPmxVertex[ i ].BoneIndex[ 3 ] = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.BoneIndexSize ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].BoneWeight[ 0 ] ) = *( ( DWORD * )&mem[ 0 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].BoneWeight[ 1 ] ) = *( ( DWORD * )&mem[ 4 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].BoneWeight[ 2 ] ) = *( ( DWORD * )&mem[ 8 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].BoneWeight[ 3 ] ) = *( ( DWORD * )&mem[ 12 ] ) ;
			mem += 16 ;
			break ;

		case 3 :	// SDEF
			m_pmx_data.s_pPmxVertex[ i ].BoneIndex[ 0 ] = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.BoneIndexSize ) ;
			m_pmx_data.s_pPmxVertex[ i ].BoneIndex[ 1 ] = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.BoneIndexSize ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].BoneWeight[ 0 ] ) = *( ( DWORD * )mem ) ;
			mem += 4 ;
			m_pmx_data.s_pPmxVertex[ i ].BoneWeight[ 1 ] = 1.0f - m_pmx_data.s_pPmxVertex[ i ].BoneWeight[ 0 ] ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].SDEF_C[ 0 ] ) = *( ( DWORD * )&mem[ 0 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].SDEF_C[ 1 ] ) = *( ( DWORD * )&mem[ 4 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].SDEF_C[ 2 ] ) = *( ( DWORD * )&mem[ 8 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].SDEF_R0[ 0 ] ) = *( ( DWORD * )&mem[ 12 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].SDEF_R0[ 1 ] ) = *( ( DWORD * )&mem[ 16 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].SDEF_R0[ 2 ] ) = *( ( DWORD * )&mem[ 20 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].SDEF_R1[ 0 ] ) = *( ( DWORD * )&mem[ 24 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].SDEF_R1[ 1 ] ) = *( ( DWORD * )&mem[ 28 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].SDEF_R1[ 2 ] ) = *( ( DWORD * )&mem[ 32 ] ) ;
			mem += 36 ;
			break ;
		}

		*( ( DWORD * )&m_pmx_data.s_pPmxVertex[ i ].ToonEdgeScale ) = *( ( DWORD * )&mem[ 0 ] ) ;
		mem += 4 ;
	}
	
	// �ʂ̐����擾
	m_pmx_data.s_PmxFaceNum = *( ( DWORD * )mem ) / 3 ;
	mem += 4 ;

	// �ʃf�[�^���i�[���郁�����̈�̊m��
	m_pmx_data.s_pPmxFace = ( PMX_FACE * )malloc( sizeof( PMX_FACE ) * m_pmx_data.s_PmxFaceNum ) ;
	if( m_pmx_data.s_pPmxFace == NULL )
	{
		printf( "PMX Load Error : �ʃf�[�^�̈ꎞ�L���̈�̊m�ۂɎ��s���܂���\n" );
		goto ENDLABEL ;
	}
	
	// �ʂ��\�����钸�_�C���f�b�N�X�̎擾
	switch( m_pmx_data.s_PmxInfo.VertexIndexSize )
	{
	case 1 :
		for( i = 0 ; ( DWORD )i < m_pmx_data.s_PmxFaceNum ; i ++ )
		{
			m_pmx_data.s_pPmxFace[ i ].VertexIndex[ 0 ] = ( int )( mem[ 0 ] ) ;
			m_pmx_data.s_pPmxFace[ i ].VertexIndex[ 1 ] = ( int )( mem[ 1 ] ) ;
			m_pmx_data.s_pPmxFace[ i ].VertexIndex[ 2 ] = ( int )( mem[ 2 ] ) ;
			mem += 3 ;
		}
		break ;

	case 2 :
		for( i = 0 ; ( DWORD )i < m_pmx_data.s_PmxFaceNum ; i ++ )
		{
			m_pmx_data.s_pPmxFace[ i ].VertexIndex[ 0 ] = ( int )( *( ( WORD * )&mem[ 0 ] ) ) ;
			m_pmx_data.s_pPmxFace[ i ].VertexIndex[ 1 ] = ( int )( *( ( WORD * )&mem[ 2 ] ) ) ;
			m_pmx_data.s_pPmxFace[ i ].VertexIndex[ 2 ] = ( int )( *( ( WORD * )&mem[ 4 ] ) ) ;
			mem += 6 ;
		}
		break ;

	case 4 :
		for( i = 0 ; ( DWORD )i < m_pmx_data.s_PmxFaceNum ; i ++ )
		{
			m_pmx_data.s_pPmxFace[ i ].VertexIndex[ 0 ] = *( ( int * )&mem[ 0 ] ) ;
			m_pmx_data.s_pPmxFace[ i ].VertexIndex[ 1 ] = *( ( int * )&mem[ 4 ] ) ;
			m_pmx_data.s_pPmxFace[ i ].VertexIndex[ 2 ] = *( ( int * )&mem[ 8 ] ) ;
			mem += 12 ;
		}
		break ;
	}

	
	// �e�N�X�`���̐����擾
	m_pmx_data.s_PmxTextureNum = *( ( DWORD * )mem ) ;
	mem += 4 ;

	// �e�N�X�`���f�[�^���i�[���郁�����̈�̊m��
	m_pmx_data.s_pPmxTexture = ( PMX_TEXTURE * )malloc( sizeof( PMX_TEXTURE ) * m_pmx_data.s_PmxTextureNum ) ;
	if( m_pmx_data.s_pPmxTexture == NULL )
	{
		printf( "PMX Load Error : �e�N�X�`���f�[�^�̈ꎞ�L���̈�̊m�ۂɎ��s���܂���\n" );
		goto ENDLABEL ;
	}

	// �e�N�X�`���̏����擾
	for( i = 0 ; ( DWORD )i < m_pmx_data.s_PmxTextureNum ; i ++ )
	{
		MV1LoadModelToPMX_GetString( &mem, m_pmx_data.s_pPmxTexture[ i ].TexturePath, m_pmx_data.s_PmxInfo.EncodeType ) ;
	}
	
	// �}�e���A���̐����擾
	m_pmx_data.s_PmxMaterialNum = *( ( DWORD * )mem ) ;
	mem += 4 ;

	// �}�e���A���f�[�^���i�[���郁�����̈�̊m��
	m_pmx_data.s_pPmxMaterial = ( PMX_MATERIAL * )malloc( sizeof( PMX_MATERIAL ) * m_pmx_data.s_PmxMaterialNum ) ;
	if( m_pmx_data.s_pPmxMaterial == NULL )
	{
		printf( "PMX Load Error : �}�e���A���f�[�^�̈ꎞ�L���̈�̊m�ۂɎ��s���܂���\n" );
		goto ENDLABEL ;
	}
	
	// �}�e���A���̓ǂ݂���
	for( i = 0 ; ( DWORD )i < m_pmx_data.s_PmxMaterialNum ; i ++ )
	{
		// �ގ����̎擾
		MV1LoadModelToPMX_GetString( &mem, m_pmx_data.s_pPmxMaterial[ i ].Name, m_pmx_data.s_PmxInfo.EncodeType ) ;
		if( m_pmx_data.s_pPmxMaterial[ i ].Name[ 0 ] == '\0' )
		{
			MV1LoadModelToPMX_GetString( &mem, m_pmx_data.s_pPmxMaterial[ i ].Name, m_pmx_data.s_PmxInfo.EncodeType ) ;
			if( m_pmx_data.s_pPmxMaterial[ i ].Name[ 0 ] == '\0' )
			{
				sprintf_s( m_pmx_data.s_pPmxMaterial[ i ].Name, "Mat_%d", i) ;
			}
		}
		else
		{
			mem += *( ( DWORD * )mem ) + 4 ;
		}

		*( ( DWORD * )&m_pmx_data.s_pPmxMaterial[ i ].Diffuse[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxMaterial[ i ].Diffuse[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxMaterial[ i ].Diffuse[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxMaterial[ i ].Diffuse[ 3 ] ) = *( ( DWORD * )&mem[ 12 ] ) ;
		mem += 16 ;

		*( ( DWORD * )&m_pmx_data.s_pPmxMaterial[ i ].Specular[ 0 ] ) = *( ( DWORD * )&mem[ 0 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxMaterial[ i ].Specular[ 1 ] ) = *( ( DWORD * )&mem[ 4 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxMaterial[ i ].Specular[ 2 ] ) = *( ( DWORD * )&mem[ 8 ] ) ;
		mem += 12 ;

		*( ( DWORD * )&m_pmx_data.s_pPmxMaterial[ i ].SpecularPower ) = *( ( DWORD * )&mem[ 0 ] ) ;
		mem += 4 ;

		*( ( DWORD * )&m_pmx_data.s_pPmxMaterial[ i ].Ambient[ 0 ] ) = *( ( DWORD * )&mem[ 0 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxMaterial[ i ].Ambient[ 1 ] ) = *( ( DWORD * )&mem[ 4 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxMaterial[ i ].Ambient[ 2 ] ) = *( ( DWORD * )&mem[ 8 ] ) ;
		mem += 12 ;

		m_pmx_data.s_pPmxMaterial[ i ].CullingOff     = ( *mem & 0x01 ) ? 1 : 0 ;
		m_pmx_data.s_pPmxMaterial[ i ].GroundShadow   = ( *mem & 0x02 ) ? 1 : 0 ;
		m_pmx_data.s_pPmxMaterial[ i ].SelfShadowMap  = ( *mem & 0x04 ) ? 1 : 0 ;
		m_pmx_data.s_pPmxMaterial[ i ].SelfShadowDraw = ( *mem & 0x08 ) ? 1 : 0 ;
		m_pmx_data.s_pPmxMaterial[ i ].EdgeDraw       = ( *mem & 0x10 ) ? 1 : 0 ;
		mem ++ ;

		*( ( DWORD * )&m_pmx_data.s_pPmxMaterial[ i ].EdgeColor[ 0 ] ) = *( ( DWORD * )&mem[ 0 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxMaterial[ i ].EdgeColor[ 1 ] ) = *( ( DWORD * )&mem[ 4 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxMaterial[ i ].EdgeColor[ 2 ] ) = *( ( DWORD * )&mem[ 8 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxMaterial[ i ].EdgeColor[ 3 ] ) = *( ( DWORD * )&mem[ 12 ] ) ;
		mem += 16 ;

		*( ( DWORD * )&m_pmx_data.s_pPmxMaterial[ i ].EdgeSize ) = *( ( DWORD * )mem ) ;
		mem += 4 ;

		m_pmx_data.s_pPmxMaterial[ i ].TextureIndex = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.TextureIndexSize ) ;
		m_pmx_data.s_pPmxMaterial[ i ].SphereTextureIndex = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.TextureIndexSize ) ;

		m_pmx_data.s_pPmxMaterial[ i ].SphereMode = *mem ;
		mem ++ ;

		m_pmx_data.s_pPmxMaterial[ i ].ToonFlag = *mem ;
		mem ++ ;
		if( m_pmx_data.s_pPmxMaterial[ i ].ToonFlag == 0 )
		{
			m_pmx_data.s_pPmxMaterial[ i ].ToonTextureIndex = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.TextureIndexSize ) ;
		}
		else
		{
			m_pmx_data.s_pPmxMaterial[ i ].ToonTextureIndex = *mem ;
			mem ++ ;
		}

		// �����̓X�L�b�v
		mem += *( ( DWORD * )mem ) + 4 ;

		m_pmx_data.s_pPmxMaterial[ i ].MaterialFaceNum = *( ( int * )mem ) ;
		mem += 4 ;
	}

	
	// �{�[���̐����擾
	m_pmx_data.s_PmxBoneNum = *( ( DWORD * )mem ) ;
	mem += 4 ;

	// �{�[���f�[�^���i�[���郁�����̈�̊m��
	m_pmx_data.s_pPmxBone = ( PMX_BONE * )malloc( sizeof( PMX_BONE ) * m_pmx_data.s_PmxBoneNum ) ;
	if( m_pmx_data.s_pPmxBone == NULL )
	{
		printf( "PMX Load Error : �{�[���f�[�^�̈ꎞ�L���̈�̊m�ۂɎ��s���܂���\n" );
		goto ENDLABEL ;
	}

	// �{�[�����̎擾
	m_pmx_data.s_PmxIKNum = 0 ;
	for( i = 0 ; ( DWORD )i < m_pmx_data.s_PmxBoneNum ; i ++ )
	{
		MV1LoadModelToPMX_GetString( &mem, m_pmx_data.s_pPmxBone[ i ].Name, m_pmx_data.s_PmxInfo.EncodeType ) ;
		if( m_pmx_data.s_pPmxBone[ i ].Name[ 0 ] == '\0' ) 
		{
			MV1LoadModelToPMX_GetString( &mem, m_pmx_data.s_pPmxBone[ i ].Name, m_pmx_data.s_PmxInfo.EncodeType ) ;
		}
		else
		{
			mem += *( ( DWORD * )mem ) + 4 ;
		}

		*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].Position[ 0 ] ) = *( ( DWORD * )&mem[ 0 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].Position[ 1 ] ) = *( ( DWORD * )&mem[ 4 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].Position[ 2 ] ) = *( ( DWORD * )&mem[ 8 ] ) ;
		mem += 12 ;

		m_pmx_data.s_pPmxBone[ i ].ParentBoneIndex = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.BoneIndexSize ) ;
		m_pmx_data.s_pPmxBone[ i ].TransformLayer = *( ( int * )mem ) ;
		mem += 4 ;

		WORD Flag = *( ( WORD * )mem ) ;
		mem += 2 ;
		m_pmx_data.s_pPmxBone[ i ].Flag_LinkDest              = ( Flag & 0x0001 ) != 0 ? 1 : 0 ;
		m_pmx_data.s_pPmxBone[ i ].Flag_EnableRot             = ( Flag & 0x0002 ) != 0 ? 1 : 0 ;
		m_pmx_data.s_pPmxBone[ i ].Flag_EnableMov             = ( Flag & 0x0004 ) != 0 ? 1 : 0 ;
		m_pmx_data.s_pPmxBone[ i ].Flag_Disp                  = ( Flag & 0x0008 ) != 0 ? 1 : 0 ;
		m_pmx_data.s_pPmxBone[ i ].Flag_EnableControl         = ( Flag & 0x0010 ) != 0 ? 1 : 0 ;
		m_pmx_data.s_pPmxBone[ i ].Flag_IK                    = ( Flag & 0x0020 ) != 0 ? 1 : 0 ;
		m_pmx_data.s_pPmxBone[ i ].Flag_AddRot                = ( Flag & 0x0100 ) != 0 ? 1 : 0 ;
		m_pmx_data.s_pPmxBone[ i ].Flag_AddMov                = ( Flag & 0x0200 ) != 0 ? 1 : 0 ;
		m_pmx_data.s_pPmxBone[ i ].Flag_LockAxis              = ( Flag & 0x0400 ) != 0 ? 1 : 0 ;
		m_pmx_data.s_pPmxBone[ i ].Flag_LocalAxis             = ( Flag & 0x0800 ) != 0 ? 1 : 0 ;
		m_pmx_data.s_pPmxBone[ i ].Flag_AfterPhysicsTransform = ( Flag & 0x1000 ) != 0 ? 1 : 0 ;
		m_pmx_data.s_pPmxBone[ i ].Flag_OutParentTransform    = ( Flag & 0x2000 ) != 0 ? 1 : 0 ;

		if( m_pmx_data.s_pPmxBone[ i ].Flag_LinkDest == 0 )
		{
			*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].OffsetPosition[ 0 ] ) = *( ( DWORD * )&mem[ 0 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].OffsetPosition[ 1 ] ) = *( ( DWORD * )&mem[ 4 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].OffsetPosition[ 2 ] ) = *( ( DWORD * )&mem[ 8 ] ) ;
			mem += 12 ;
		}
		else
		{
			m_pmx_data.s_pPmxBone[ i ].LinkBoneIndex = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.BoneIndexSize ) ;
		}

		if( m_pmx_data.s_pPmxBone[ i ].Flag_AddRot == 1 || m_pmx_data.s_pPmxBone[ i ].Flag_AddMov == 1 )
		{
			m_pmx_data.s_pPmxBone[ i ].AddParentBoneIndex = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.BoneIndexSize ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].AddRatio ) = *( ( DWORD * )mem ) ;
			mem += 4 ;
		}

		if( m_pmx_data.s_pPmxBone[ i ].Flag_LockAxis == 1 )
		{
			*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].LockAxisVector[ 0 ] ) = *( ( DWORD * )&mem[ 0 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].LockAxisVector[ 1 ] ) = *( ( DWORD * )&mem[ 4 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].LockAxisVector[ 2 ] ) = *( ( DWORD * )&mem[ 8 ] ) ;
			mem += 12 ;
		}

		if( m_pmx_data.s_pPmxBone[ i ].Flag_LocalAxis == 1 )
		{
			*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].LocalAxisXVector[ 0 ] ) = *( ( DWORD * )&mem[ 0 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].LocalAxisXVector[ 1 ] ) = *( ( DWORD * )&mem[ 4 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].LocalAxisXVector[ 2 ] ) = *( ( DWORD * )&mem[ 8 ] ) ;
			mem += 12 ;

			*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].LocalAxisZVector[ 0 ] ) = *( ( DWORD * )&mem[ 0 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].LocalAxisZVector[ 1 ] ) = *( ( DWORD * )&mem[ 4 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].LocalAxisZVector[ 2 ] ) = *( ( DWORD * )&mem[ 8 ] ) ;
			mem += 12 ;
		}

		if( m_pmx_data.s_pPmxBone[ i ].Flag_OutParentTransform == 1 )
		{
			m_pmx_data.s_pPmxBone[ i ].OutParentTransformKey = *( ( int * )mem ) ;
			mem += 4 ;
		}

		if( m_pmx_data.s_pPmxBone[ i ].Flag_IK == 1 )
		{
			m_pmx_data.s_PmxIKNum ++ ;

			m_pmx_data.s_pPmxBone[ i ].IKInfo.TargetBoneIndex = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.BoneIndexSize ) ;
			m_pmx_data.s_pPmxBone[ i ].IKInfo.LoopNum = *( ( int * )mem ) ;
			mem += 4 ;

			*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].IKInfo.RotLimit ) = *( ( DWORD * )mem ) ;
			mem += 4 ;

			m_pmx_data.s_pPmxBone[ i ].IKInfo.LinkNum = *( ( int * )mem ) ;
			mem += 4 ;
			if( m_pmx_data.s_pPmxBone[ i ].IKInfo.LinkNum >= PMX_MAX_IKLINKNUM )
			{
				printf( "PMX Load Error : �h�j�����N�̐����Ή����𒴂��Ă��܂�\n" );
				goto ENDLABEL ;
			}

			for( j = 0 ; j < m_pmx_data.s_pPmxBone[ i ].IKInfo.LinkNum ; j ++ )
			{
				m_pmx_data.s_pPmxBone[ i ].IKInfo.Link[ j ].BoneIndex = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.BoneIndexSize ) ;
				m_pmx_data.s_pPmxBone[ i ].IKInfo.Link[ j ].RotLockFlag = mem[ 0 ] ;
				mem ++ ;

				if( m_pmx_data.s_pPmxBone[ i ].IKInfo.Link[ j ].RotLockFlag == 1 )
				{
					*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].IKInfo.Link[ j ].RotLockMin[ 0 ] ) = *( ( DWORD * )&mem[ 0 ] ) ;
					*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].IKInfo.Link[ j ].RotLockMin[ 1 ] ) = *( ( DWORD * )&mem[ 4 ] ) ;
					*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].IKInfo.Link[ j ].RotLockMin[ 2 ] ) = *( ( DWORD * )&mem[ 8 ] ) ;
					mem += 12 ;

					*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].IKInfo.Link[ j ].RotLockMax[ 0 ] ) = *( ( DWORD * )&mem[ 0 ] ) ;
					*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].IKInfo.Link[ j ].RotLockMax[ 1 ] ) = *( ( DWORD * )&mem[ 4 ] ) ;
					*( ( DWORD * )&m_pmx_data.s_pPmxBone[ i ].IKInfo.Link[ j ].RotLockMax[ 2 ] ) = *( ( DWORD * )&mem[ 8 ] ) ;
					mem += 12 ;
				}
			}
		}
	}

	
	// ���[�t���̐����擾
	m_pmx_data.s_PmxMorphNum = *( ( DWORD * )mem ) ;
	mem += 4 ;

	// ���[�t�f�[�^���i�[���郁�����̈�̊m��
	m_pmx_data.s_pPmxMorph = ( PMX_MORPH * )malloc( sizeof( PMX_MORPH ) * m_pmx_data.s_PmxMorphNum ) ;
	if( m_pmx_data.s_pPmxMorph == NULL )
	{
		printf( "PMX Load Error : ���[�t�f�[�^�̈ꎞ�L���̈�̊m�ۂɎ��s���܂���\n" );
		goto ENDLABEL ;
	}

	// ���[�t���̓ǂݍ���
	m_pmx_data.s_PmxSkinNum = 0 ;
	for( i = 0 ; ( DWORD )i < m_pmx_data.s_PmxMorphNum ; i ++ )
	{
		MV1LoadModelToPMX_GetString( &mem, m_pmx_data.s_pPmxMorph[ i ].Name, m_pmx_data.s_PmxInfo.EncodeType ) ;
		if( m_pmx_data.s_pPmxMorph[ i ].Name[ 0 ] == '\0' ) 
		{
			MV1LoadModelToPMX_GetString( &mem, m_pmx_data.s_pPmxMorph[ i ].Name, m_pmx_data.s_PmxInfo.EncodeType ) ;
		}
		else
		{
			mem += *( ( DWORD * )mem ) + 4 ;
		}

		m_pmx_data.s_pPmxMorph[ i ].ControlPanel = mem[ 0 ] ;
		m_pmx_data.s_pPmxMorph[ i ].Type = mem[ 1 ] ;
		m_pmx_data.s_pPmxMorph[ i ].DataNum = *( ( int * )&mem[ 2 ] ) ;
		mem += 6 ;

		switch( m_pmx_data.s_pPmxMorph[ i ].Type )
		{
		case 0 :	// �O���[�v���[�t
			m_pmx_data.s_pPmxMorph[ i ].Group = ( PMX_MORPH_GROUP * )malloc( sizeof( PMX_MORPH_GROUP ) * m_pmx_data.s_pPmxMorph[ i ].DataNum) ;
			if( m_pmx_data.s_pPmxMorph[ i ].Group == NULL )
			{
				printf( "PMX Load Error : �O���[�v���[�t�f�[�^�̈ꎞ�L���̈�̊m�ۂɎ��s���܂���\n" );
				goto ENDLABEL ;
			}

			for( j = 0 ; j < m_pmx_data.s_pPmxMorph[ i ].DataNum ; j ++ )
			{
				m_pmx_data.s_pPmxMorph[ i ].Group[ j ].Index = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.MorphIndexSize ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Group[ j ].Ratio ) = *( ( DWORD * )mem ) ;
				mem += 4 ;
			}
			break ;

		case 1 :	// ���_
			m_pmx_data.s_PmxSkinNum ++ ;
			m_pmx_data.s_pPmxMorph[ i ].Vertex = ( PMX_MORPH_VERTEX * )malloc( sizeof( PMX_MORPH_VERTEX ) * m_pmx_data.s_pPmxMorph[ i ].DataNum) ;
			if( m_pmx_data.s_pPmxMorph[ i ].Vertex == NULL )
			{
				printf( "PMX Load Error : ���_���[�t�f�[�^�̈ꎞ�L���̈�̊m�ۂɎ��s���܂���\n" );
				goto ENDLABEL ;
			}

			for( j = 0 ; j < m_pmx_data.s_pPmxMorph[ i ].DataNum ; j ++ )
			{
				m_pmx_data.s_pPmxMorph[ i ].Vertex[ j ].Index = MV1LoadModelToPMX_GetUInt( &mem, m_pmx_data.s_PmxInfo.VertexIndexSize ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Vertex[ j ].Offset[ 0 ] ) = *( ( DWORD * )&mem[ 0 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Vertex[ j ].Offset[ 1 ] ) = *( ( DWORD * )&mem[ 4 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Vertex[ j ].Offset[ 2 ] ) = *( ( DWORD * )&mem[ 8 ] ) ;
				mem += 12 ;
			}
			break ;

		case 2 :	// �{�[�����[�t
			m_pmx_data.s_pPmxMorph[ i ].Bone = ( PMX_MORPH_BONE * )malloc( sizeof( PMX_MORPH_BONE ) * m_pmx_data.s_pPmxMorph[ i ].DataNum ) ;
			if( m_pmx_data.s_pPmxMorph[ i ].Bone == NULL )
			{
				printf( "PMX Load Error : �{�[�����[�t�f�[�^�̈ꎞ�L���̈�̊m�ۂɎ��s���܂���\n" );
				goto ENDLABEL ;
			}

			for( j = 0 ; j < m_pmx_data.s_pPmxMorph[ i ].DataNum ; j ++ )
			{
				m_pmx_data.s_pPmxMorph[ i ].Bone[ j ].Index = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.BoneIndexSize ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Bone[ j ].Offset[ 0 ] ) = *( ( DWORD * )&mem[ 0 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Bone[ j ].Offset[ 1 ] ) = *( ( DWORD * )&mem[ 4 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Bone[ j ].Offset[ 2 ] ) = *( ( DWORD * )&mem[ 8 ] ) ;
				mem += 12 ;

				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Bone[ j ].Quat[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Bone[ j ].Quat[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Bone[ j ].Quat[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Bone[ j ].Quat[ 3 ] ) = *( ( DWORD * )&mem[ 12 ] ) ;
				mem += 16 ;
			}
			break ;

		case 3 :	// UV���[�t
		case 4 :	// �ǉ�UV1���[�t
		case 5 :	// �ǉ�UV2���[�t
		case 6 :	// �ǉ�UV3���[�t
		case 7 :	// �ǉ�UV4���[�t
			m_pmx_data.s_pPmxMorph[ i ].UV = ( PMX_MORPH_UV * )malloc( sizeof( PMX_MORPH_UV ) * m_pmx_data.s_pPmxMorph[ i ].DataNum ) ;
			if( m_pmx_data.s_pPmxMorph[ i ].UV == NULL )
			{
				printf( "PMX Load Error : UV���[�t�f�[�^�̈ꎞ�L���̈�̊m�ۂɎ��s���܂���\n" );
				goto ENDLABEL ;
			}

			for( j = 0 ; j < m_pmx_data.s_pPmxMorph[ i ].DataNum ; j ++ )
			{
				m_pmx_data.s_pPmxMorph[ i ].UV[ j ].Index = MV1LoadModelToPMX_GetUInt( &mem, m_pmx_data.s_PmxInfo.VertexIndexSize ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].UV[ j ].Offset.x ) = *( ( DWORD * )&mem[  0 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].UV[ j ].Offset.y ) = *( ( DWORD * )&mem[  4 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].UV[ j ].Offset.z ) = *( ( DWORD * )&mem[  8 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].UV[ j ].Offset.w ) = *( ( DWORD * )&mem[ 12 ] ) ;
				mem += 16 ;
			}
			break ;

		case 8 :	// �ގ����[�t
			m_pmx_data.s_pPmxMorph[ i ].Material = ( PMX_MORPH_MATERIAL * )malloc( sizeof( PMX_MORPH_MATERIAL ) * m_pmx_data.s_pPmxMorph[ i ].DataNum ) ;
			if( m_pmx_data.s_pPmxMorph[ i ].Material == NULL )
			{
				printf( "PMX Load Error : �}�e���A�����[�t�f�[�^�̈ꎞ�L���̈�̊m�ۂɎ��s���܂���\n" );
				goto ENDLABEL ;
			}

			for( j = 0 ; j < m_pmx_data.s_pPmxMorph[ i ].DataNum ; j ++ )
			{
				m_pmx_data.s_pPmxMorph[ i ].Material[ j ].Index = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.MaterialIndexSize ) ;
				m_pmx_data.s_pPmxMorph[ i ].Material[ j ].CalcType = mem[ 0 ] ;
				mem ++ ;

				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].Diffuse[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].Diffuse[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].Diffuse[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].Diffuse[ 3 ] ) = *( ( DWORD * )&mem[ 12 ] ) ;
				mem += 16 ;

				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].Specular[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].Specular[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].Specular[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
				mem += 12 ;

				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].SpecularPower ) = *( ( DWORD * )&mem[  0 ] ) ;
				mem += 4 ;

				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].Ambient[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].Ambient[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].Ambient[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
				mem += 12 ;

				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].EdgeColor[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].EdgeColor[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].EdgeColor[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].EdgeColor[ 3 ] ) = *( ( DWORD * )&mem[ 12 ] ) ;
				mem += 16 ;

				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].EdgeSize ) = *( ( DWORD * )&mem[  0 ] ) ;
				mem += 4 ;

				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].TextureScale[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].TextureScale[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].TextureScale[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].TextureScale[ 3 ] ) = *( ( DWORD * )&mem[ 12 ] ) ;
				mem += 16 ;

				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].SphereTextureScale[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].SphereTextureScale[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].SphereTextureScale[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].SphereTextureScale[ 3 ] ) = *( ( DWORD * )&mem[ 12 ] ) ;
				mem += 16 ;

				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].ToonTextureScale[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].ToonTextureScale[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].ToonTextureScale[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
				*( ( DWORD * )&m_pmx_data.s_pPmxMorph[ i ].Material[ j ].ToonTextureScale[ 3 ] ) = *( ( DWORD * )&mem[ 12 ] ) ;
				mem += 16 ;
			}
			break ;
		}
	}


	// �\���g�f�[�^�̓ǂݍ��݂̓X�L�b�v
	{
		DWORD PmxDispFrameNum ;
		DWORD Num ;
		BYTE Target ;

		// �\���g�f�[�^�̐����擾
		PmxDispFrameNum = *( ( DWORD * )mem ) ;
		mem += 4 ;

		// �g�̐������J��Ԃ�
		for( i = 0 ; ( DWORD )i < PmxDispFrameNum ; i ++ )
		{
			// ���O�ǂݔ�΂�
			mem += *( ( DWORD * )mem ) + 4 ;
			mem += *( ( DWORD * )mem ) + 4 ;

			// ����g�t���O�ǂݔ�΂�
			mem ++ ;

			// �g���v�f�̐����擾
			Num = *( ( DWORD * )mem ) ;
			mem += 4 ;
			
			// �g���v�f�̓ǂݔ�΂�
			for( j = 0 ; ( DWORD )j < Num ; j ++ )
			{
				// �v�f�Ώۂ��擾
				Target = mem[ 0 ] ;
				mem ++ ;

				// �v�f�Ώۂɂ���ď����𕪊�
				switch( Target )
				{
				case 0 :	// �{�[�����^�[�Q�b�g�̏ꍇ
					MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.BoneIndexSize ) ;
					break ;

				case 1 :	// ���[�t���^�[�Q�b�g�̏ꍇ
					MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.MorphIndexSize ) ;
					break ;
				}
			}
		}
	}


	// ���̏��̐����擾
	m_pmx_data.s_PmxRigidbodyNum = *( ( DWORD * )mem ) ;
	mem += 4 ;

	// ���̃f�[�^���i�[���郁�����̈�̊m��
	m_pmx_data.s_pPmxRigidbody = ( PMX_RIGIDBODY * )malloc( sizeof( PMX_RIGIDBODY ) * m_pmx_data.s_PmxRigidbodyNum ) ;
	if( m_pmx_data.s_pPmxRigidbody == NULL )
	{
		printf( "PMX Load Error : ���̃f�[�^�̈ꎞ�L���̈�̊m�ۂɎ��s���܂���\n" );
		goto ENDLABEL ;
	}

	// ���̏��̓ǂݍ���
	for( i = 0 ; ( DWORD )i < m_pmx_data.s_PmxRigidbodyNum ; i++ )
	{
		MV1LoadModelToPMX_GetString( &mem, m_pmx_data.s_pPmxRigidbody[ i ].Name, m_pmx_data.s_PmxInfo.EncodeType ) ;
		if( m_pmx_data.s_pPmxRigidbody[ i ].Name[ 0 ] == '\0' ) 
		{
			MV1LoadModelToPMX_GetString( &mem, m_pmx_data.s_pPmxRigidbody[ i ].Name, m_pmx_data.s_PmxInfo.EncodeType ) ;
		}
		else
		{
			mem += *( ( DWORD * )mem ) + 4 ;
		}

		m_pmx_data.s_pPmxRigidbody[ i ].BoneIndex = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.BoneIndexSize ) ;

		m_pmx_data.s_pPmxRigidbody[ i ].RigidBodyGroupIndex = mem[ 0 ] ;
		mem ++ ;

		*( ( WORD * )&m_pmx_data.s_pPmxRigidbody[ i ].RigidBodyGroupTarget ) = *( ( WORD * )&mem[ 0 ] ) ;
		mem += 2 ;

		m_pmx_data.s_pPmxRigidbody[ i ].ShapeType = mem[ 0 ] ;
		mem ++ ;

		*( ( DWORD * )&m_pmx_data.s_pPmxRigidbody[ i ].ShapeW ) = *( ( DWORD * )&mem[ 0 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxRigidbody[ i ].ShapeH ) = *( ( DWORD * )&mem[ 4 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxRigidbody[ i ].ShapeD ) = *( ( DWORD * )&mem[ 8 ] ) ;
		mem += 12 ;

		*( ( DWORD * )&m_pmx_data.s_pPmxRigidbody[ i ].Position[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxRigidbody[ i ].Position[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxRigidbody[ i ].Position[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
		mem += 12 ;

		*( ( DWORD * )&m_pmx_data.s_pPmxRigidbody[ i ].Rotation[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxRigidbody[ i ].Rotation[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxRigidbody[ i ].Rotation[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
		mem += 12 ;

		*( ( DWORD * )&m_pmx_data.s_pPmxRigidbody[ i ].RigidBodyWeight ) = *( ( DWORD * )&mem[  0 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxRigidbody[ i ].RigidBodyPosDim ) = *( ( DWORD * )&mem[  4 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxRigidbody[ i ].RigidBodyRotDim ) = *( ( DWORD * )&mem[  8 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxRigidbody[ i ].RigidBodyRecoil ) = *( ( DWORD * )&mem[ 12 ] ) ;
		*( ( DWORD * )&m_pmx_data.s_pPmxRigidbody[ i ].RigidBodyFriction ) = *( ( DWORD * )&mem[ 16 ] ) ;
		mem += 20 ;

		m_pmx_data.s_pPmxRigidbody[ i ].RigidBodyType = mem[ 0 ] ;
		mem ++ ;
	}


	// �W���C���g���̐����擾
	m_pmx_data.s_PmxJointNum = *( ( DWORD * )mem ) ;
	mem += 4 ;

	// �W���C���g�f�[�^���i�[���郁�����̈�̊m��
	if( m_pmx_data.s_PmxJointNum != 0 )
	{
		m_pmx_data.s_pPmxJoint = ( PMX_JOINT * )malloc( sizeof( PMX_JOINT ) * m_pmx_data.s_PmxJointNum ) ;
		if( m_pmx_data.s_pPmxJoint == NULL )
		{
			printf( "PMX Load Error : �W���C���g�f�[�^�̈ꎞ�L���̈�̊m�ۂɎ��s���܂���\n" );
			goto ENDLABEL ;
		}

		// �W���C���g���̓ǂݍ���
		for( i = 0 ; ( DWORD )i < m_pmx_data.s_PmxJointNum ; i ++ )
		{
			MV1LoadModelToPMX_GetString( &mem, m_pmx_data.s_pPmxJoint[ i ].Name, m_pmx_data.s_PmxInfo.EncodeType ) ;
			if( m_pmx_data.s_pPmxJoint[ i ].Name[ 0 ] == '\0' ) 
			{
				MV1LoadModelToPMX_GetString( &mem, m_pmx_data.s_pPmxJoint[ i ].Name, m_pmx_data.s_PmxInfo.EncodeType ) ;
			}
			else
			{
				mem += *( ( DWORD * )mem ) + 4 ;
			}

			m_pmx_data.s_pPmxJoint[ i ].Type = mem[ 0 ] ;
			mem ++ ;

			m_pmx_data.s_pPmxJoint[ i ].RigidBodyAIndex = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.RigidIndexSize ) ;
			m_pmx_data.s_pPmxJoint[ i ].RigidBodyBIndex = MV1LoadModelToPMX_GetInt( &mem, m_pmx_data.s_PmxInfo.RigidIndexSize ) ;

			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].Position[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].Position[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].Position[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
			mem += 12 ;

			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].Rotation[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].Rotation[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].Rotation[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
			mem += 12 ;

			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].ConstrainPositionMin[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].ConstrainPositionMin[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].ConstrainPositionMin[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
			mem += 12 ;

			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].ConstrainPositionMax[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].ConstrainPositionMax[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].ConstrainPositionMax[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
			mem += 12 ;

			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].ConstrainRotationMin[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].ConstrainRotationMin[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].ConstrainRotationMin[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
			mem += 12 ;

			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].ConstrainRotationMax[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].ConstrainRotationMax[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].ConstrainRotationMax[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
			mem += 12 ;

			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].SpringPosition[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].SpringPosition[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].SpringPosition[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
			mem += 12 ;

			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].SpringRotation[ 0 ] ) = *( ( DWORD * )&mem[  0 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].SpringRotation[ 1 ] ) = *( ( DWORD * )&mem[  4 ] ) ;
			*( ( DWORD * )&m_pmx_data.s_pPmxJoint[ i ].SpringRotation[ 2 ] ) = *( ( DWORD * )&mem[  8 ] ) ;
			mem += 12 ;
		}
	}

	free(mem_head);
	return true;
ENDLABEL :
	free(mem_head);
	return false;
}