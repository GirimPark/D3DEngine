#include "Shared.fxh"
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;

    float4x4 matWorld;
#ifdef VERTEX_SKINNING
    matWorld = mul(input.BlendWeights.x, BoneMatrix[input.BlendIndices.x]);
    matWorld += mul(input.BlendWeights.y, BoneMatrix[input.BlendIndices.y]);
    matWorld += mul(input.BlendWeights.z, BoneMatrix[input.BlendIndices.z]);
    matWorld += mul(input.BlendWeights.w, BoneMatrix[input.BlendIndices.w]);
#else
	matWorld = NodeWorld;
#endif

	output.Pos = mul(input.Pos, matWorld);
    output.PixelPos = output.Pos.xyz;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Texture = input.Texture;
    output.Tangent = normalize(mul(input.Tangent, (float3x3)matWorld));
    output.Normal = normalize(mul(input.Normal, (float3x3)matWorld));
    output.BiTangent = cross(output.Normal, output.Tangent);
    return output;
} 