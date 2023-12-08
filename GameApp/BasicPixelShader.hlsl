#include "Shared.fxh"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

static const float PI = 3.141592;
static const float Epsilon = 0.00001;

static const float3 Fdielectric = 0.04;

// Cook-Torrance Specular BRDF 인자 함수
float ndfGGX(float cosNH, float roughness)
{  
    // D, 법선 분포 함수
	float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    // 디즈니 방식, alpha = roughness^2
    float denom = cosNH*cosNH*(alphaSq - 1.0) + 1.0;
    return alphaSq / (PI*denom*denom);
}

float fresnelSchlick(float3 F0, float cosVH)
{
	// F, 프레넬-슐릭 방정식
    return (F0 + (1.0 - F0) * (1.0 - pow(cosVH, 5.0)));
}

float gaSchlickG1(float cosTheta, float k)
{
	return ( cosTheta / (cosTheta * (1 - k) + k) );
}

float gaSchlickGGX(float cosNL, float cosNH, float roughness)
{
	// G, 기하학적 감쇠
    float alpha = roughness + 1.0;
    float k = (alpha * alpha) / 8.0;    // Epic
    return gaSchlickG1(cosNL, k) * gaSchlickG1(cosNH, k);
}

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
    //float3 ambientColor =  AmbientPower * albedoColor;

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

    float3 prePBRColor = saturate(diffuseColor + specularColor + emissiveColor);

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

    /// PBR
    // Specular BRDF
    float roughness = 0.5f;
    float metalness = 0.3f;

    float D = ndfGGX(cosNH, roughness);

    float cosLH = max(0.f, dot(viewVector, halfVector));
    float3 F0 = lerp(Fdielectric, albedoColor, metalness);
    float3 F = fresnelSchlick(F0, cosLH);

    float cosNL = max(0.f, dot(normal, -lightDir));
    float G = gaSchlickGGX(cosNL, cosNH, roughness);

    // Diffuse BRDF
    float 

    /// 최종 색상은 
    float3 finalColor;
    finalColor = pow(finalColor, 1/2.2);

    return float4(finalColor, opacityColor);
}
