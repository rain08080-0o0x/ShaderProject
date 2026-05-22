
struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPos : TEXCOORD1; // サーフェイルの光が当たった場所
};

// 点光源のデータを受け取る定数バッファ
cbuffer PointLight : register(b0)
{
    float3 Lpos;    // 点光源の座標
    float Lrange;   // 点根源までの距離
    float4 Lcolor;  // 光源色
}

SamplerState samp : register(s0);
Texture2D tex : register(t0);

float4 main(PS_IN pin) : SV_TARGET
{
    // 1.サーフェイスへの光の入射角計算(P.119
    float3 L = pin.wPos - Lpos;
    L = normalize(L);

    // 2.距離の影響率の計算(P.120
    float dist = length(pin.wPos - Lpos);

    // 影響率の式(P.111)に割り当てて計算
    float affect = 1.0f - 1.0f / Lrange * dist;
    affect = saturate(affect);

    // 3.影響力を指数関数的にする(P.120)
    affect = pow(affect, 3.0f);
    
    
    // 上記の計算は点光源からの明るさのみの計算
    // そもそもモデルのどの部分に陰ができるかは
    // 法線とライトから計算を行う
    float3 N = normalize(pin.normal);
    float d = dot(N,L);
    d *= -1.0f;
    d = saturate(d);

    // テクスチャ、点光源、拡散反射から
    // 最終的な色を計算 
    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    color = tex.Sample(samp, pin.uv);
    color.rgb *= Lcolor.rgb * d * affect;
    return color;
}
