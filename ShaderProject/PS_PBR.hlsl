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
    float dummy1;
    float4 Lcolor;
    float4 Lambient;
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
    float t2 = t * t;
    float t4 = t * t * t * t;
    float m2 = m * m;
    float D = 1.0f / (4.0f * m2 * t4);
    D *= exp((-1.0f / m2) * (1.0f - t2) / t2);
    return D;
}

// G項の計算
float funcG(float NdotH,float NdotV,float NdotL, float VdotH)
{
    float g1 = 2.0f * NdotH * NdotV / VdotH;
    float g2 = 2.0f * NdotH * NdotL / VdotH;
    float G = min(1.0f, min(g1, g2));
    return G;
}

// F項をシュリックの近似値で計算
float Schlick(float f0, float u)
{
    return f0 + (1.0f - f0) * pow(1.0f - u, 5.0f);
}

// Cook-Torranceモデルの鏡面反射を計算
// L - 光源に向かうベクトル
// V - 視点に向かうベクトル
// N - 法線
// metalic - 金属度。金属度が高いほど、鏡面反射の色が光源の色に近づく。
float CookTorrance(float3 L,float3 V,float3 N,float metalic)
{
    // 表面の粗さを計算(0～1で表す
    float microfacet = 0.76f;
    
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
    float D = Beckmann(microfacet, NdotH);
    
    // G項(幾何減衰)の計算
    float G = funcG(NdotH, NdotV, NdotL, VdotH);
    
    // F項(フレネル)の計算
    float F = Schlick(f0, VdotH);
    
    // 分母の計算
    float m = 3.141592f * NdotV * NdotH;
    
    return max(D * G * F / m, 0.0f);
}

float4 main(PS_IN pin) : SV_TARGET
{
    // 各種パラメータの計算
    float3 N = normalize(pin.normal);
    float3 L = normalize(-Ldir);
    float3 V = normalize(Cpos - pin.wPos);

    float metalic = 0.5f; // 金属度合い
    float smooth = 0.5f; // なめらかさ
    float4 color = tex.Sample(samp, pin.uv);
    
    // ディスニーベースの拡散反射(P.182
    float NdotL = saturate(dot(N, L));
    float3 diffuse = color.rgb * NdotL;
    
    // Cook-Torranceの鏡面反射(P.186
    float3 spec = CookTorrance(L, V, N, smooth);
    spec *= Lcolor.rgb;
    // 金属度が高いと反射に色が付き、低いと白くなる
    spec = lerp(spec, spec * color.rgb, metalic);
    
    // 滑らかさを使って、拡散反射と鏡面反射を合成
    color.rgb = diffuse * (1.0f - smooth) + spec;
    color.rgb = saturate(color.rgb);
    
    return color;

}
