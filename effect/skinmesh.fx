
float4x4 world[50];	// 定数レジスタは256個 → (256 - 4 - 4- 1 - 1)/4 = 61のボーン行列がMAX
float4x4 view;
float4x4 proj;
float4 ambient;
float4 diffuse;
float4 specular;
float power;
float lightbloompower = 0.0f;

float3 camerapos;

texture tex;

sampler TextureSampler = sampler_state {
	Texture = <tex>;
};

// ライト方向
float3 lightDir;

struct VS_IN {
	float3 pos : POSITION;
	float3 blend : BLENDWEIGHT; 	
	int4 idx : BLENDINDICES;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD0;
};

struct VS_OUT {
	float4 pos : POSITION;
	float4 color : COLOR0;
	float2 uv : TEXCOORD0;
	float3 N : TEXCOORD1;
	float3	L	: TEXCOORD2;	// ライト方向
	float3	E	: TEXCOORD3;	// 
};

// 頂点シェーダ
VS_OUT BlendVS(VS_IN In) {
	VS_OUT Out = (VS_OUT)0;
	float w[3] = (float[3])In.blend;
	float4x4 comb = (float4x4)0;
	for (int i = 0; i < 3; ++i) comb += world[In.idx[i]]*w[i];
	comb += world[In.idx[3]] * (1.0f - w[0] - w[1] - w[2]);
	float4 pos = mul( float4(In.pos, 1.0), comb );
	
	Out.N = mul(In.normal, (float3x3)comb );
	Out.N = normalize(Out.N);
	
	Out.L = -lightDir;
	Out.L = normalize(Out.L);
	
	Out.pos = pos;
	Out.pos = mul(Out.pos, view);
	Out.pos = mul(Out.pos, proj);
	Out.color = ambient;
	
	Out.E = camerapos-pos;
	Out.E = normalize(Out.E);
	
	Out.uv = In.uv;
	return Out;
}

struct PS_OUTPUT
{
   float4 Col1 : COLOR0;  //シーンのカラー情報
   float4 Col2 : COLOR1;  //シーンの輝度情報
};

// ピクセルシェーダ
PS_OUTPUT BlendPS(VS_OUT In, uniform bool b_tex)
{
	PS_OUTPUT Out;
	float4 out_color;
	float4 S;
	float3 R;
	
	R = reflect(-In.L, In.N );
	S = specular * pow(max(dot(In.E, R), 0.0f), power );
	
	/*if (b_tex){
		float4 tex_color = tex2D(TextureSampler, In.uv);
		// テクスチャの色と頂点の色を掛け合わせる
		out_color = (In.color+(diffuse*max(dot(In.N, In.L), 0.0f)))*tex_color+S;
		Out.Col1 = out_color;
		
		// Specularをセットする
		float p = S * lightbloompower;
		Out.Col2 = float4(p, p, p, 1.0f);
	}
	else{
		out_color = In.color+(diffuse*max(dot(In.N, In.L), 0.0f))+S;
		Out.Col1 = out_color;
		// Specularをセットする
		float p = S * lightbloompower;
		Out.Col2 = float4(p, p, p, 1.0f);
	}*/
	
	float4 tex_color = tex2D(TextureSampler, In.uv);
	// テクスチャの色と頂点の色を掛け合わせる
	out_color = (In.color+(diffuse*max(dot(In.N, In.L), 0.0f)))*tex_color+S;
	Out.Col1 = out_color;
	
	// Specularをセットする
	float p = S * lightbloompower;
	Out.Col2 = float4(p, p, p, 1.0f);

	return Out;
}

// テクニック
technique BlendTech {
	pass P0	{
		VertexShader = compile vs_3_0 BlendVS();
		PixelShader = compile ps_3_0 BlendPS(true);
	}
	pass P1	{
		VertexShader = compile vs_3_0 BlendVS();
		PixelShader = compile ps_3_0 BlendPS(false);
	}
}