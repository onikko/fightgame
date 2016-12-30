/*============================================================
	BasicPhongSpecular

--------------------------------------------------------------
	����ҁF�X�@��^
	������F2013/08/28
	�ǋL���F2015/01/21
============================================================*/

//------------------------------------------------
// �O���[�o���ϐ�
//------------------------------------------------
// ���W�ϊ��p
float4x4	g_world;					// ���[���h�ϊ��s��
float4x4 	g_view;						// �J�����ϊ��s��
float4x4 	g_projection;				// �v���W�F�N�V�����ϊ��s��

// �e�N�X�`��
texture		g_texture;					// �e�N�X�`��
bool		g_tex;						// �e�N�X�`���̂���Ȃ��@false:�Ȃ��@true:����

// ��
float4		g_diffuse;					// �f�B�t���[�Y
float4		g_emmisive;					// �G�~�b�V�u
float4		g_ambient;					// ����
float4		g_specular;					// �X�y�L�����[��
float3		g_light_dir;				// ���s�����̕���

// �}�e���A��
float4		g_diffuse_mat;				// �f�B�t���[�Y��
float4		g_emmisive_mat;				// �G�~�b�V�u��
float4		g_ambient_mat;				// ����
float4		g_specular_mat;				// �X�y�L�����[
float		g_power;					// �X�y�L�����[���̃p���[�l

// �J�����̈ʒu
float3		g_camerapos;			

float4 m_FogColor = float4(1.0f, 1.0f, 1.0f, 1.0f);                  //�t�H�O�J���[
float  m_Near   = 60.0f;             //�t�H�O�̊J�n�ʒu
float  m_Far    = 200.0f;             //�t�H�O�̏I���ʒu
//------------------------------------------------
// �T���v���[1
//------------------------------------------------
sampler TextureSampler1 =
sampler_state{
	Texture = <g_texture>;
	MinFilter= LINEAR;		// ���j�A�t�B���^�i�k�����j
	MagFilter= LINEAR;		// ���j�A�t�B���^�i�g�厞�j
};

struct	GouraudVS_OUTPUT
{
	float4	Pos	: POSITION;		// ���W�l
	float4	Col	: COLOR0;		// ���_�F
	float2	Tex	: TEXCOORD0;	// UV���W
	float3	N	: TEXCOORD1;	// �@��
	float3	L	: TEXCOORD2;	// ���C�g����
	float3	E	: TEXCOORD3;	// 
	float4  WVP : TEXCOORD4;
};

//------------------------------------------------
// ���_�V�F�[�_
//------------------------------------------------
GouraudVS_OUTPUT GouraudVS( float4 in_pos : POSITION,
							float4 in_normal : NORMAL,
							float2 in_tex1	: TEXCOORD0 )
{
	GouraudVS_OUTPUT Out;
	
	// ���W�ϊ�
	Out.Pos = mul(in_pos, g_world);
	Out.WVP = Out.Pos;
	// P=out_pos;
	Out.Pos = mul(Out.Pos, g_view);
	Out.Pos = mul(Out.Pos, g_projection);
	// �e�N�X�`�����W�����̂܂܏o�͂���
	Out.Tex = in_tex1;
	
	Out.N = mul(in_normal.xyz, (float3x3)g_world);
	Out.N = normalize(Out.N);
	
	Out.L = -g_light_dir.xyz;
	Out.L = normalize(Out.L);
	// ���C�g�x�N�g���Ɩ@���x�N�g���̓��ς��v�Z���A�@���x�N�g���̓��ς��v�Z���A�v�Z���ʂ̐F���Œ����Ambient�Ƃ��Đݒ�B
	//Out.Col = g_ambient*g_ambient_mat;
	Out.Col = g_ambient_mat;
	
	// ���_-> ���_�ւ̃x�N�g�����v�Z
	Out.E = g_camerapos.xyz - in_pos.xyz;
	Out.E = normalize(Out.E);
	return Out;
}
//------------------------------------------------
// �s�N�Z���V�F�[�_
//------------------------------------------------
float4 GouraudPS( GouraudVS_OUTPUT In ) : COLOR0
{
	float4 out_color;
	float4 S;
	float3 R;
	
	R = reflect(-In.L, In.N );
	S = g_specular * pow(max(dot(In.E, R), 0.0f),g_power);
	   
   //���_�Ǝ��_�Ƃ̋������v�Z����
	float d = distance( In.WVP.xyz, g_camerapos.xyz );
	float f = ( m_Far - d ) / ( m_Far - m_Near );
	f = clamp( f, 0.0f, 1.0f );
	//f=0.5f;
	if(g_tex){
		float4 tex_color= tex2D(TextureSampler1,In.Tex );
		// �e�N�X�`���̐F�ƒ��_�̐F���|�����킹��
		//out_color = (In.Col+(g_diffuse*g_diffuse_mat*max(dot(In.N, In.L), 0.0f )))*tex_color+S;
		out_color = (In.Col+(g_diffuse_mat*max(dot(In.N, In.L), 0.0f )))*tex_color+S * f + m_FogColor * (1.0f-f);
		//out_color = tex_color * f + m_FogColor * ( 1.0f - f );
	}
	else{
		out_color = In.Col+(g_diffuse*g_diffuse_mat*max(dot(In.N, In.L), 0.0f ))+S;
	}
	
	return out_color;
}
//------------------------------------------------
// �e�N�j�b�N�錾
//------------------------------------------------
technique BasicTech
{
    pass P0
    	{
        	vertexShader = compile vs_3_0 GouraudVS();
        	pixelShader  = compile ps_3_0 GouraudPS();
		}
}
