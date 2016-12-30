/*============================================================
	BasicPhongSpecular

--------------------------------------------------------------
	制作者：森　一真
	制作日：2013/08/28
	追記日：2015/01/21
============================================================*/

//------------------------------------------------
// グローバル変数
//------------------------------------------------
// 座標変換用
float4x4	g_world;					// ワールド変換行列
float4x4 	g_view;						// カメラ変換行列
float4x4 	g_projection;				// プロジェクション変換行列

// テクスチャ
texture		g_texture;					// テクスチャ
bool		g_tex;						// テクスチャのありなし　false:なし　true:あり

// 光
float4		g_diffuse;					// ディフューズ
float4		g_emmisive;					// エミッシブ
float4		g_ambient;					// 環境光
float4		g_specular;					// スペキュラー光
float3		g_light_dir;				// 平行光源の方向

// マテリアル
float4		g_diffuse_mat;				// ディフューズ光
float4		g_emmisive_mat;				// エミッシブ光
float4		g_ambient_mat;				// 環境光
float4		g_specular_mat;				// スペキュラー
float		g_power;					// スペキュラー光のパワー値

// カメラの位置
float3		g_camerapos;			

float4 m_FogColor = float4(1.0f, 1.0f, 1.0f, 1.0f);                  //フォグカラー
float  m_Near   = 60.0f;             //フォグの開始位置
float  m_Far    = 200.0f;             //フォグの終了位置
//------------------------------------------------
// サンプラー1
//------------------------------------------------
sampler TextureSampler1 =
sampler_state{
	Texture = <g_texture>;
	MinFilter= LINEAR;		// リニアフィルタ（縮小時）
	MagFilter= LINEAR;		// リニアフィルタ（拡大時）
};

struct	GouraudVS_OUTPUT
{
	float4	Pos	: POSITION;		// 座標値
	float4	Col	: COLOR0;		// 頂点色
	float2	Tex	: TEXCOORD0;	// UV座標
	float3	N	: TEXCOORD1;	// 法線
	float3	L	: TEXCOORD2;	// ライト方向
	float3	E	: TEXCOORD3;	// 
	float4  WVP : TEXCOORD4;
};

//------------------------------------------------
// 頂点シェーダ
//------------------------------------------------
GouraudVS_OUTPUT GouraudVS( float4 in_pos : POSITION,
							float4 in_normal : NORMAL,
							float2 in_tex1	: TEXCOORD0 )
{
	GouraudVS_OUTPUT Out;
	
	// 座標変換
	Out.Pos = mul(in_pos, g_world);
	Out.WVP = Out.Pos;
	// P=out_pos;
	Out.Pos = mul(Out.Pos, g_view);
	Out.Pos = mul(Out.Pos, g_projection);
	// テクスチャ座標をそのまま出力する
	Out.Tex = in_tex1;
	
	Out.N = mul(in_normal.xyz, (float3x3)g_world);
	Out.N = normalize(Out.N);
	
	Out.L = -g_light_dir.xyz;
	Out.L = normalize(Out.L);
	// ライトベクトルと法線ベクトルの内積を計算し、法線ベクトルの内積を計算し、計算結果の色を最低限のAmbientとして設定。
	//Out.Col = g_ambient*g_ambient_mat;
	Out.Col = g_ambient_mat;
	
	// 頂点-> 視点へのベクトルを計算
	Out.E = g_camerapos.xyz - in_pos.xyz;
	Out.E = normalize(Out.E);
	return Out;
}
//------------------------------------------------
// ピクセルシェーダ
//------------------------------------------------
float4 GouraudPS( GouraudVS_OUTPUT In ) : COLOR0
{
	float4 out_color;
	float4 S;
	float3 R;
	
	R = reflect(-In.L, In.N );
	S = g_specular * pow(max(dot(In.E, R), 0.0f),g_power);
	   
   //頂点と視点との距離を計算する
	float d = distance( In.WVP.xyz, g_camerapos.xyz );
	float f = ( m_Far - d ) / ( m_Far - m_Near );
	f = clamp( f, 0.0f, 1.0f );
	//f=0.5f;
	if(g_tex){
		float4 tex_color= tex2D(TextureSampler1,In.Tex );
		// テクスチャの色と頂点の色を掛け合わせる
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
// テクニック宣言
//------------------------------------------------
technique BasicTech
{
    pass P0
    	{
        	vertexShader = compile vs_3_0 GouraudVS();
        	pixelShader  = compile ps_3_0 GouraudPS();
		}
}
