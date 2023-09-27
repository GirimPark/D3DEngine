#include "Shared.fxh"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float3 lightDir = normalize(LightDirection.xyz);
    float3 normal = normalize(input.Normal);
    float3 colorFactor = LightColor.xyz * txDiffuse.Sample(samLinear, input.Texture);

    float3 finalColor = saturate(dot(normal, -lightDir) * colorFactor);

    return float4(finalColor, 1.f);
}
