#include "Shared.fxh"
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT main(float4 Pos : POSITION, float4 Color : COLOR, float4 Normal : NORMAL)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Color = Color;
    output.Normal = mul(Normal, World);
    return output;
}