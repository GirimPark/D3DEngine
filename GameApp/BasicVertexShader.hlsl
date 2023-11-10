#include "Shared.fxh"
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(input.Pos, NodeWorld);
    output.PixelPos = output.Pos.xyz;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Texture = input.Texture;
    output.Tangent = normalize(mul(input.Tangent, (float3x3)NodeWorld));
    output.Normal = normalize(mul(input.Normal, (float3x3)NodeWorld));
    output.BiTangent = cross(output.Normal, output.Tangent);
    return output;
} 