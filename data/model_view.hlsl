
float4x4 g_WorldViewProjMatrix;

texture  g_DiffuseTex;
sampler  g_DiffuseTexSampler = sampler_state
{
    Texture   = <g_DiffuseTex>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

struct VS_OUTPUT
{
    float4 Position    : POSITION;    // vertex position
    float2 TextureUV1  : TEXCOORD0;   // vertex texture coords 1
    float4 Diffuse     : COLOR0;      // vertex diffuse color
};

VS_OUTPUT RenderSceneVS( float4 vPos       : POSITION,
                         float3 vNormal    : NORMAL,
                         float4 vDiffuse   : COLOR0,
                         float2 vTexCoord0 : TEXCOORD0,
                         float2 vTexCoord1 : TEXCOORD1)
{
    VS_OUTPUT o  = (VS_OUTPUT)0;
    o.Position   = mul(vPos, g_WorldViewProjMatrix);
    o.TextureUV1 = vTexCoord0;
    o.Diffuse    = vDiffuse;
    return o;
}

struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;
};

PS_OUTPUT RenderScenePS(VS_OUTPUT In)
{
    PS_OUTPUT o;
    o.RGBColor.rgb = tex2D(g_DiffuseTexSampler, In.TextureUV1).rgb * In.Diffuse.rgb;
    o.RGBColor.a   = 1;
    return o;
};

technique RenderScene
{
    pass P0
    {
        VertexShader = compile vs_2_0 RenderSceneVS();
        PixelShader  = compile ps_2_0 RenderScenePS();
    }
}
