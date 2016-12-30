/////////////////////////////////////////////////////////////////
// ポリライン描画
/////////////////////////////////////////////////////////////////

texture		texLine;			// ラインテクスチャ
float4x4	matWVP;				// 変換行列

// 頂点シェーダ入力
struct VS_INPUT{
	float4	vPos	: POSITION;
	float2	vTex	: TEXCOORD0;
};

// 頂点シェーダ出力
struct VS_OUTPUT{
	float4	vPos	: POSITION;
	float2	vTex	: TEXCOORD0;
};

sampler s0 = sampler_state{
	texture = <texLine>;
	AddressU = WRAP;
	AddressV = WRAP;
};

// 頂点シェーダ
VS_OUTPUT vsDraw(const VS_INPUT v)
{
	VS_OUTPUT	o = (VS_OUTPUT)0;

	// 座標変換
	o.vPos = mul( v.vPos, matWVP );

	// UV座標はそのまま
	o.vTex = v.vTex;

	return o;
}



// ピクセルシェーダ
float4 psPointDraw(const VS_OUTPUT v) : COLOR
{
	float4	col = {1.0f, 1.0f, 1.0f, 1.0f};
	return col;
}

struct PS_OUTPUT
{
   float4 Col1 : COLOR0;  //シーンのカラー情報
   float4 Col2 : COLOR1;  //シーンの輝度情報
};

PS_OUTPUT psPolyLineDraw(const VS_OUTPUT v)
{
	PS_OUTPUT Out;
	Out.Col1 = tex2D( s0, v.vTex );
	Out.Col2 = Out.Col1 * 5.0f ;
	return Out;
}

// テクニック
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
