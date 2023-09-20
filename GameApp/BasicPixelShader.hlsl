#include "Shared.fxh"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float3 lightDir = normalize(LightDirection.xyz);
    float3 lightColor = LightColor.xyz;
    float intensity = LightIntensity;

    float4 lightFactor = saturate(dot(input.Normal.xyz, lightDir));

    float3 finalColor = input.Color.xyz * lightColor * intensity * lightFactor;

    return float4(finalColor, 1.f);
}
