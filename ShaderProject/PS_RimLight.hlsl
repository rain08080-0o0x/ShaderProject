struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPos : TEXCOORD1; // サーフェイルの光が当たった場所
};

cbuffer Light : register(b0)
{
    float3 lightPos;
    float dummy0;
    
    float3 Ldir;
    float Ldummy;
    
    float4 Lcolor;
    
    float4 Lambient;
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
    float3 L = normalize(Ldir);
    float3 V = normalize(pin.wPos - Cpos); // 視線の方向

    // 1.法線と光の方向に依存するリムの強さ(P.141
    float power1 = 1.0f - max(0.0f, dot(L, N));
    // 2.法線と視線の方向に依存するリムの強さ(P.142
    float power2 = 1.0f - max(0.0f, dot(-V, N));
    
    // 3.最終的なリムの強さを求める(P.142
    float rimPower = power1 * power2;
    rimPower = pow(rimPower, 1.3f);
    
    // 4.最終的な反射光にリムライトの光を合算する
    float3 rimColor = rimPower * Lcolor.rgb;
    
    float4 color = tex.Sample(samp, pin.uv);
    color.rgb += rimColor;
    

    return color;
}
