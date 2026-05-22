struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPos : TEXCOORD1; // サーフェイルの光が当たった場所
};

// スポットライトのデータを受け取る定数バッファ
cbuffer SpotLight : register(b0)
{
    float3 Lpos;    // ｽﾎﾟｯﾄﾗｲﾄの座標
    float Lrange;   // ｽﾎﾟｯﾄﾗｲﾄまでの距離
    float4 Lcolor;  // ｽﾎﾟｯﾄﾗｲﾄの光の色
    // 点光源から新しく増えたパラメーター
    float3 Ldir;    // ｽﾎﾟｯﾄﾗｲﾄのテラス方向
    float Langle;   // ｽﾎﾟｯﾄﾗｲﾄの照らす範囲(角度)
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
    
    // 4.光の向きとテラス方向の角度を求める(P.134
    float3 Sdir = normalize(Ldir);
    float angle = dot(L, Sdir);
    // 内積の性質から、二つのベクトルの間の角度を求める
    // 内積計算後、acos関数を通すことで計算できる
    angle = abs(acos(angle));

    angle = angle / 3.1415926535F * 180;
    
    // 5.角度のより影響率を求める(P.134
    // ｽﾎﾟｯﾄﾗｲﾄの範囲外担うrと急激に暗くなるのではなく、
    // 徐々に暗くなるように切り替わる。点光源の距離の
    // 影響と同じ考えで、角度についても影響率を計算する
    float Saffect = 1.0f - 1.0f / Langle * angle;
    Saffect = saturate(Saffect);
    Saffect = pow(Saffect, 0.5f);

    // 上記の計算は点光源からの明るさのみの計算
    // そもそもモデルのどの部分に陰ができるかは
    // 法線とライトから計算を行う
    float3 N = normalize(pin.normal);
    float d = dot(N, L);
    d *= -1.0f;
    d = saturate(d);

    // テクスチャ、点光源、拡散反射から
    // 最終的な色を計算 
    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    color = tex.Sample(samp, pin.uv);
    color.rgb *= d * affect * Saffect * Lcolor.rgb;
    return color;
}
