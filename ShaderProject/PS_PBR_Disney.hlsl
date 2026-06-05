#define PI 3.1415926535f

struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPos : TEXCOORD1;
};

cbuffer Light : register(b0)
{
    float3 lightPos;
    float dummy0;
    float3 Ldir;
    float dummy1;
    float4 Lcolor;
    float4 Lambient;
}

cbuffer Camera : register(b1)
{
    float3 Cpos;
    float dummy;
}

cbuffer PBRParam : register(b2)
{
    // x = metallic, y = roughness, z = specular, w = baseColorPower
    float4 PBRParams;
}

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float SchlickWeight(float u)
{
    float m = saturate(1.0f - u);
    float m2 = m * m;
    return m2 * m2 * m;
}

float DisneyDiffuse(float NdotV, float NdotL, float LdotH, float roughness)
{
    // Disney diffuse: roughnessÇ™çÇÇ¢ÇŸÇ«êÛÇ¢äpìxÇÃägéUîΩéÀÇè≠Çµã≠ÇﬂÇÈÅB
    float fd90 = 0.5f + 2.0f * LdotH * LdotH * roughness;
    float lightScatter = lerp(1.0f, fd90, SchlickWeight(NdotL));
    float viewScatter = lerp(1.0f, fd90, SchlickWeight(NdotV));
    return lightScatter * viewScatter / PI;
}

float DistributionGGX(float NdotH, float roughness)
{
    float a = max(roughness * roughness, 0.001f);
    float a2 = a * a;
    float denom = NdotH * NdotH * (a2 - 1.0f) + 1.0f;
    return a2 / max(PI * denom * denom, 0.0001f);
}

float GeometrySchlickGGX(float NdotX, float roughness)
{
    float r = roughness + 1.0f;
    float k = (r * r) / 8.0f;
    return NdotX / max(NdotX * (1.0f - k) + k, 0.0001f);
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

float3 FresnelSchlick(float VdotH, float3 F0)
{
    return F0 + (1.0f - F0) * SchlickWeight(VdotH);
}

float4 main(PS_IN pin) : SV_TARGET
{
    float4 texColor = tex.Sample(samp, pin.uv);
    float3 baseColor = saturate(texColor.rgb * PBRParams.w);

    float metallic = saturate(PBRParams.x);
    float roughness = saturate(PBRParams.y);
    float specular = saturate(PBRParams.z);

    float3 N = normalize(pin.normal);
    float3 L = normalize(-Ldir);
    float3 V = normalize(Cpos - pin.wPos);
    float3 H = normalize(L + V);

    float NdotL = saturate(dot(N, L));
    float NdotV = saturate(dot(N, V));
    float NdotH = saturate(dot(N, H));
    float VdotH = saturate(dot(V, H));
    float LdotH = saturate(dot(L, H));

    float diffuseTerm = DisneyDiffuse(NdotV, NdotL, LdotH, roughness);
    float3 diffuse = baseColor * diffuseTerm * NdotL * Lcolor.rgb * (1.0f - metallic);

    float3 F0 = lerp(float3(0.08f * specular, 0.08f * specular, 0.08f * specular), baseColor, metallic);
    float D = DistributionGGX(NdotH, roughness);
    float G = GeometrySmith(NdotV, NdotL, roughness);
    float3 F = FresnelSchlick(VdotH, F0);
    float3 specularLight = (D * G * F) / max(4.0f * NdotV * NdotL, 0.0001f);
    specularLight *= NdotL * Lcolor.rgb;

    float3 ambient = baseColor * Lambient.rgb;
    float3 finalColor = ambient + diffuse + specularLight;

    return float4(saturate(finalColor), texColor.a);
}