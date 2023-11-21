//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
Texture2D txSpecular : register(t2);
Texture2D txEmissive : register(t3);
Texture2D txOpacity : register(t4);

SamplerState samLinear : register(s0);

cbuffer TransformBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

cbuffer LightingBuffer : register(b1)
{
    float4 LightDirection;
    float4 LightColor;
    float3 CameraTranslation;
    float LightIntensity;
    float AmbientPower;
    float SpecularPower;
}

cbuffer TexturingBuffer : register(b2)
{
    bool UseDiffuse;
    bool UseNormal;
    bool UseSpecular;
    bool UseEmissive;
    bool UseOpacity;
}

cbuffer NodeTransformBuffer : register(b3)
{
    matrix NodeWorld;
}

cbuffer BoneMatrixBuffer : register(b4)
{
    matrix BoneMatrix[128];
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Texture : TEXTURE;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    int4 BlendIndices : BLENDINDICES;
    float4 BlendWeights : BLENDWEIGHTS;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Texture : TEXCOORD0;
    float3 PixelPos : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiTangent : TEXCOORD3;
    float3 Normal : TEXCOORD4;
};