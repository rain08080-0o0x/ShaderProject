struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPos : TEXCOORD1; // サーフェイルの光が当たった場所
};

cbuffer Light : register(b0)
{
    float3 Lpos;
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
// ランプテクスチャ
Texture2D rampTex : register(t1);

float4 main(PS_IN pin) :SV_TARGET
{
    // 0.事前の情報を取得
    float3 L = normalize(Ldir);
    float3 N = normalize(pin.normal);
    
    // 1.法線とライトから陰を計算
    float d = dot(L, N);
    d = saturate(-d);

    // ifを使用して内積の結果が特定の値以上であれば明るくする。
    // ifを使用して内積の結果が特定の未満であれば暗くする。
    //if (d >= 0.3f)
    //{
    //    d = 1.0f;
    //}
    //else
    //{
    //    d = 0.3f;
    //}
    
    // そもそもシェーダー内であまりifを使用したくないので、
    // 画像を使った方法で明るさを設定
    // 内積の計算結果をそのまま画像の参照位置として利用
    // テクスチャの補間設定でUVが0(1)だと繰り返された隣のテクスチャを参照するため、
    // 明るくならない部分が明るくなってしまう。UVを0～1にするのではなく、
    // 0.01 ～ 0.99 にして補完の結果を利用しないようにする。※本来はDirectXの設定を変えるべき
    d = d * 0.98f + 0.01f;

    
    float brightness = rampTex.Sample(samp, float2(d, 0.5f)).r;
    d = brightness;
    
    // 2.陰の結果をモデルに適用
    float4 color = tex.Sample(samp, pin.uv);
    color.rgb *= d;
    
    return color;
}