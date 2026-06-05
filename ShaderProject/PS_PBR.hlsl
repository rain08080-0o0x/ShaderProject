struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPos : POSITION; // サーフェイルの光が当たった場所
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
cbuffer Camera : register(b1)
{
    float3 Cpos;
    float dummy;
}

Texture2D tex : register(t0);
SamplerState samp : register(s0);

// D項をベックマン分布を用いて計算
float Beckmann(float m,float t)
{
    return 0.0f;
}

// G項の計算
float funcG(float NdotH,float NdotV,float NdotL, float VdotH)
{
    return 0.0f;
}

// F項をシュリックの近似値で計算
float Schlick(float f0, float u)
{
    return 0.0f;
}

// Cool-Traranceモデルの鏡面反射を計算
// L - 光源に向かうベクトル
// V - 視点に向かうベクトル
// N - 法線
// metalic - 金属度。金属度が高いほど、鏡面反射の色が光源の色に近づく。
float CoolTrrance(float3 L,float3 V,float3 N,float3 metalic)
{
    // 表面の粗さを計算(0～1で表す
    float micorofacet = 0.76f;
    
    // 金属度を垂直入射時のフレネル反射率とする
    float f0 = metalic;

    // ライトと視線のハーフベクトル
    float3 H = normalize(L + V);
    
    // 各種ベクトル同士の近似度を計算
    float NdotH = saturate(dot(N, H));
    float VdotH = saturate(dot(V, H));
    float NdotL = saturate(dot(N, L));
    float NdotV = saturate(dot(N, V));
    
    // D頂(マイクロファセット)の計算
    
    // G項(幾何減衰)の計算
    
    // F項(フレネル)の計算


    return float(1.0f);
}

float4 main(PS_IN pin) : SV_TARGET
{
    
    return float4(0.0f, 0.0f, 0.0f, 0.0f);

}
