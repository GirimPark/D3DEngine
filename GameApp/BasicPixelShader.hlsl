#include "Shared.fxh"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 main(PS_INPUT input) : SV_Target
{
    /// NormalMap Mapping
    float3 normal;
    if(UseNormal)
    {
	    float3 tangentSpace = txNormal.Sample(samLinear, input.Texture).rgb * 2.f - 1.f;
	    input.Tangent = normalize(input.Tangent);
	    input.BiTangent = normalize(input.BiTangent);
	    input.Normal = normalize(input.Normal);
	    float3x3 normalTransform = float3x3(input.Tangent, input.BiTangent, input.Normal);
	    normal = normalize(mul(tangentSpace, normalTransform));
    }
    else
    {
        normal = normalize(input.Normal);
    }
    
    /// Lighting
    float3 albedoColor;
    if(UseDiffuse)
    {
		albedoColor = txDiffuse.Sample(samLinear, input.Texture).rgb;
    }
    else
    {
        albedoColor = BaseColor;
    }
    albedoColor.rgb = pow(albedoColor, 2.2);

    /// SpecularMap Mapping
    float3 specularMapColor;
    if (UseSpecular)
    {
        specularMapColor = txSpecular.Sample(samLinear, input.Texture).rgb;
    }
    else
    {
        specularMapColor = albedoColor;
    }

    // ambient
    float3 ambientColor =  AmbientPower * albedoColor;

    // diffuse
    float3 lightDir = normalize(LightDirection.xyz);
    float3 colorFactor = LightColor.xyz * albedoColor;
    float3 diffuseColor = saturate(dot(normal, -lightDir) * colorFactor) * LightIntensity;

    // specular(Blinn Phong)
    float3 viewVector = normalize(CameraTranslation - input.PixelPos);
    float3 halfVector = normalize(-lightDir + viewVector);
    float cosNH = max(0.f, dot(normal, halfVector));
    float3 specularColor = pow(cosNH, SpecularPower) * LightColor.xyz * specularMapColor;

    // emissive
    float3 emissiveColor;
    if(UseEmissive)
    {
		emissiveColor = txEmissive.Sample(samLinear, input.Texture).rgb;
    }
    else
    {
	    emissiveColor = float3(0.f, 0.f, 0.f);
    }

    float3 finalColor = saturate(ambientColor + diffuseColor + specularColor + emissiveColor);
    finalColor = pow(finalColor, 1 / 2.2);

    // opacity
    float opacityColor;
    if(UseOpacity)
    {
	    opacityColor = txOpacity.Sample(samLinear, input.Texture).a;
    }
    else
    {
	    opacityColor = 1.f;
    }

    return float4(finalColor, opacityColor);
}
