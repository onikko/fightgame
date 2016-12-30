float m_TU[5];   //X方向の隣のテクセル位置
float m_TV[5];   //Y方向の隣のテクセル位置

sampler s0 : register(s0);  //ブラーをかける元となるテクスチャー

struct VS_OUTPUT
{
   float4 Pos    : POSITION;
   float2 Tex    : TEXCOORD0;
};

VS_OUTPUT VS( float4 Pos    : POSITION,
              float2 Tex    : TEXCOORD0 )
{
   VS_OUTPUT Out;
   
   Out.Pos = Pos; //行列変換しないのでそのままピクセルシェーダーに渡す
   Out.Tex = Tex;
   
   return Out;
}

//X方向にぼかす
float4 PS1( VS_OUTPUT In ) : COLOR0
{
   //テクセルを取得   
   float2 Texel0 = In.Tex + float2( -m_TU[0], 0.0f );
   float2 Texel1 = In.Tex + float2( -m_TU[1], 0.0f );
   float2 Texel2 = In.Tex + float2( -m_TU[2], 0.0f );
   float2 Texel3 = In.Tex + float2( -m_TU[3], 0.0f );
   float2 Texel4 = In.Tex + float2( -m_TU[4], 0.0f );
      
   float2 Texel5 = In.Tex + float2( m_TU[0], 0.0f );
   float2 Texel6 = In.Tex + float2( m_TU[1], 0.0f );
   float2 Texel7 = In.Tex + float2( m_TU[2], 0.0f );
   float2 Texel8 = In.Tex + float2( m_TU[3], 0.0f );
   float2 Texel9 = In.Tex + float2( m_TU[4], 0.0f );

   //取得したテクセル位置のカラー情報を取得する。
   //それぞれのカラー値に重みをかけている。この重み値はすべての合計が 1.0f になるように調整する。
   float4 p0  = tex2D( s0, In.Tex ) * 0.20f;
      
   float4 p1  = tex2D( s0, Texel0 ) * 0.12f;
   float4 p2  = tex2D( s0, Texel1 ) * 0.10f;
   float4 p3  = tex2D( s0, Texel2 ) * 0.08f;
   float4 p4  = tex2D( s0, Texel3 ) * 0.06f;      
   float4 p5  = tex2D( s0, Texel4 ) * 0.04f;
   
   float4 p6  = tex2D( s0, Texel5 ) * 0.12f;
   float4 p7  = tex2D( s0, Texel6 ) * 0.10f;
   float4 p8  = tex2D( s0, Texel7 ) * 0.08f;
   float4 p9  = tex2D( s0, Texel8 ) * 0.06f;
   float4 p10 = tex2D( s0, Texel9 ) * 0.04f;

   //カラーを合成する
   return p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9 + p10;
}

//Y方向にぼかす
float4 PS2( VS_OUTPUT In ) : COLOR0
{
   //テクセルを取得   
   float2 Texel0 = In.Tex + float2( 0.0f, -m_TV[0] );
   float2 Texel1 = In.Tex + float2( 0.0f, -m_TV[1] );
   float2 Texel2 = In.Tex + float2( 0.0f, -m_TV[2] );
   float2 Texel3 = In.Tex + float2( 0.0f, -m_TV[3] );
   float2 Texel4 = In.Tex + float2( 0.0f, -m_TV[4] );
      
   float2 Texel5 = In.Tex + float2( 0.0f, m_TV[0] );
   float2 Texel6 = In.Tex + float2( 0.0f, m_TV[1] );
   float2 Texel7 = In.Tex + float2( 0.0f, m_TV[2] );
   float2 Texel8 = In.Tex + float2( 0.0f, m_TV[3] );
   float2 Texel9 = In.Tex + float2( 0.0f, m_TV[4] );

   //取得したテクセル位置のカラー情報を取得する。
   //それぞれのカラー値に重みをかけている。この重み値はすべての合計が 1.0f になるように調整する。
   float4 p0  = tex2D( s0, In.Tex ) * 0.20f;
      
   float4 p1  = tex2D( s0, Texel0 ) * 0.12f;
   float4 p2  = tex2D( s0, Texel1 ) * 0.10f;
   float4 p3  = tex2D( s0, Texel2 ) * 0.08f;
   float4 p4  = tex2D( s0, Texel3 ) * 0.06f;      
   float4 p5  = tex2D( s0, Texel4 ) * 0.04f;
   
   float4 p6  = tex2D( s0, Texel5 ) * 0.12f;
   float4 p7  = tex2D( s0, Texel6 ) * 0.10f;
   float4 p8  = tex2D( s0, Texel7 ) * 0.08f;
   float4 p9  = tex2D( s0, Texel8 ) * 0.06f;
   float4 p10 = tex2D( s0, Texel9 ) * 0.04f;

   //カラーを合成する
   return p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9 + p10;
}

technique TShader
{
   pass P0
   {
      VertexShader = compile vs_1_1 VS();
      PixelShader  = compile ps_2_0 PS1();
   }
   
   pass P1
   {
      VertexShader = compile vs_1_1 VS();
      PixelShader  = compile ps_2_0 PS2();
   }   
}

