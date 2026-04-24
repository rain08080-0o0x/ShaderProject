//頂点シェーダーから送られてくるデータの定義
struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;     // 法線
};

// 定数バッファ
// モデルに含まれていないデータを受け取る
cbuffer Light : register(b0)
{
    float3 Ldir;    // ライトの向き
    float dummy;
    float4 Lcolor;  // ライトの色
}

float4 main( PS_IN pin) : SV_TARGET
{
    // 1.法線とライトのベクトルで内積を計算

    // 2.-1を乗算して結果を反転させる

    // 3.計算した結果を使ってライトの計算

	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}