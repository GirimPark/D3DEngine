#include "Shared.fxh"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    ///// NormalMap Mapping
    //float3 tangentSpace = txNormal.Sample(samLinear, input.Texture).rgb * 2.f - 1.f;
    //input.Tangent = normalize(input.Tangent);
    //input.BiTangent = normalize(input.BiTangent);
    //input.Normal = normalize(input.Normal);
    //float3x3 normalTransform = float3x3(input.Tangent, input.BiTangent, input.Normal);
    //float3 normal = normalize(mul(tangentSpace, normalTransform));


    ///// SpecularMap Mapping
    //float3 specularMapColor = txSpecular.Sample(samLinear, input.Texture).rgb;

    ///// Lighting
    //float3 materialColor = txDiffuse.Sample(samLinear, input.Texture);

    //// ambient
    //float3 ambientColor =  AmbientPower * materialColor;   // * ambientLightColor

    //// diffuse
    //float3 lightDir = normalize(LightDirection.xyz);
    //float3 colorFactor = LightColor.xyz * materialColor;
    //float3 diffuseColor = saturate(dot(normal, -lightDir) * colorFactor) * LightIntensity;

    //// specular(Blinn Phong)
    ////CameraTranslation = mul(float4(CameraTranslation, 1.f), World);
    //float3 viewVector = normalize(CameraTranslation - input.PixelPos);
    //float3 halfVector = normalize(-lightDir + viewVector);
    //float fHDotN = max(0.f, dot(halfVector, normal));
    //float3 specularColor = pow(fHDotN, SpecularPower) * LightColor.xyz * specularMapColor;

    //float3 finalColor = saturate(ambientColor + diffuseColor + specularColor);

    //return float4(finalColor, 1.f);

    float4 textureColor = txDiffuse.Sample(samLinear, input.Texture);

    return textureColor;
}
