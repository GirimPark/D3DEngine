//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
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
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Texture : TEXCOORD0;
    float3 Normal : NORMAL;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Texture : TEXCOORD0;
    float3 Normal : TEXCOORD1;
};