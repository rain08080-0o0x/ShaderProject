struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPos : POSITION;// サーフェイルの光が当たった場所
};

cbuffer Light : register(b0)
{
    float3 lightPos;
    float dummy0;
    float4 Lcolor;
    float4 Lambient;
    float3 eyePos;
    float dummy1;
}

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(PS_IN pin) : SV_TARGET
{
    float4 texColor = tex.Sample(samp, pin.uv);

    float3 N = normalize(pin.normal);
    float3 L = normalize(lightPos - pin.wPos);
    float3 V = normalize(eyePos - pin.wPos);
    float3 R = reflect(-L, N);

    float diffuse = saturate(dot(N, L));
    float specular = pow(saturate(dot(R, V)), 64.0f) * diffuse;

    float3 baseLight = Lcolor.rgb * diffuse * 0.75f;
    float3 highlight = Lcolor.rgb * specular * 1.5f;
    float3 ambient = Lambient.rgb * 0.01f;

    float3 color = texColor.rgb * (baseLight + ambient) + highlight;

    return float4(saturate(color), texColor.a);
}
