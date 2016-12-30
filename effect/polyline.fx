/////////////////////////////////////////////////////////////////
// �|�����C���`��
/////////////////////////////////////////////////////////////////

texture		texLine;			// ���C���e�N�X�`��
float4x4	matWVP;				// �ϊ��s��

// ���_�V�F�[�_����
struct VS_INPUT{
	float4	vPos	: POSITION;
	float2	vTex	: TEXCOORD0;
};

// ���_�V�F�[�_�o��
struct VS_OUTPUT{
	float4	vPos	: POSITION;
	float2	vTex	: TEXCOORD0;
};

sampler s0 = sampler_state{
	texture = <texLine>;
	AddressU = WRAP;
	AddressV = WRAP;
};

// ���_�V�F�[�_
VS_OUTPUT vsDraw(const VS_INPUT v)
{
	VS_OUTPUT	o = (VS_OUTPUT)0;

	// ���W�ϊ�
	o.vPos = mul( v.vPos, matWVP );

	// UV���W�͂��̂܂�
	o.vTex = v.vTex;

	return o;
}



// �s�N�Z���V�F�[�_
float4 psPointDraw(const VS_OUTPUT v) : COLOR
{
	float4	col = {1.0f, 1.0f, 1.0f, 1.0f};
	return col;
}

struct PS_OUTPUT
{
   float4 Col1 : COLOR0;  //�V�[���̃J���[���
   float4 Col2 : COLOR1;  //�V�[���̋P�x���
};

PS_OUTPUT psPolyLineDraw(const VS_OUTPUT v)
{
	PS_OUTPUT Out;
	Out.Col1 = tex2D( s0, v.vTex );
	Out.Col2 = Out.Col1 * 5.0f ;
	return Out;
}

// �e�N�j�b�N
technique Tec0_PointDraw
{
	pass Pas0
	{
		VertexShader = compile vs_2_0 vsDraw();
		PixelShader = compile ps_2_0 psPointDraw();

		ZEnable = TRUE;
		ZWriteEnable = FALSE;
		AlphaBlendEnable = TRUE;
	}
}

technique Tec1_PolyLineDraw
{
	pass Pas0
	{
		VertexShader = compile vs_2_0 vsDraw();
		PixelShader = compile ps_3_0 psPolyLineDraw();

		ZEnable = TRUE;
		ZWriteEnable = TRUE;
		AlphaBlendEnable = TRUE;
		CullMode = None;
	}
}
