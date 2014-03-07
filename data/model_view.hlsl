float4x4 g_WorldViewProjMatrix;

texture  g_DiffuseTex;
sampler  g_DiffuseTexSampler = sampler_state {
	Texture = <g_DiffuseTex>;
	MipFilter = ANISOTROPIC;
	MinFilter = ANISOTROPIC;
	MagFilter = ANISOTROPIC;
};

struct VS_OUTPUT {
	float4 Position    : POSITION;    // vertex position
	float2 TextureUV1  : TEXCOORD0;   // vertex texture coords 1
};

VS_OUTPUT RenderSceneVS( float4 vPos       : POSITION,
	float4 vNormal : NORMAL,
	float2 vTexCoord0 : TEXCOORD0 ) {
	VS_OUTPUT o = ( VS_OUTPUT ) 0;
	o.Position = mul( vPos, g_WorldViewProjMatrix );
	o.TextureUV1 = vTexCoord0;
	return o;
}

struct PS_OUTPUT {
	float4 RGBColor : COLOR0;
};

PS_OUTPUT RenderScenePS( VS_OUTPUT In ) {
	PS_OUTPUT o;

	float4 tColor = tex2D( g_DiffuseTexSampler, In.TextureUV1 ).rgba;
		float alpha = min( tColor.a * 2.f, 1.f );
	o.RGBColor.rgb = tColor.rgb;
	o.RGBColor.a = alpha;

	return o;
};

technique RenderScene {
	pass P0 {
		VertexShader = compile vs_2_0 RenderSceneVS( );
		PixelShader = compile ps_2_0 RenderScenePS( );
	}
}
