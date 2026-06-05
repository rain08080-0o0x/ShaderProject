#define RIM_LIGHT_MAX 3

struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPos : TEXCOORD1;
};
cbuffer RimLight : register(b0)
{
    float4 Ldir[RIM_LIGHT_MAX];
    float4 Lcolor[RIM_LIGHT_MAX];
    float4 Lambient;
    float4 RimInfo; // x = 使用するリムライト数
}

cbuffer Camera : register(b1)
{
    float3 Cpos;
    float Cdummy;
}

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(PS_IN pin) : SV_TARGET
{
    float3 N = normalize(pin.normal);
    float3 V = normalize(Cpos - pin.wPos);
    
    float3 rimColor = 0.0f;
    float3 specularColor = 0.0f;
    
    int rimCount = (int) RimInfo.x;
    float specularPower = RimInfo.y;
    float shininess = RimInfo.z;
    
    for (int i = 0; i < rimCount;i++)
    {
        float3 L = normalize(Ldir[i].xyz);

        // リムライト
        float rimPower = 1.0f - saturate(dot(V, N));
        rimPower = pow(rimPower, 3.0f);
        
        rimColor += rimPower * Lcolor[i].rgb;

        // 鏡面反射
        float diffuseGate = saturate(dot(L, N));
        
        float3 H = normalize(L + V);
        float specular = pow(saturate(dot(N, H)), shininess);
        
        specularColor += specular * diffuseGate * Lcolor[i].rgb * specularPower;
    }

    float4 color = tex.Sample(samp, pin.uv);
    
    float3 ambientColor = color.rgb * Lambient.rgb;
    
    color.rgb = ambientColor + rimColor + specularColor;
    color.rgb = saturate(color.rgb);
    
    return color;
}