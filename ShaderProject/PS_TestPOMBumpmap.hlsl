// 頂点シェーダーから送られてくるデータの定義
struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0; // 法線
    float3 wPos : TEXCOORD1;
    float3 tangent : TANGENT; // 接ベクトル
};

cbuffer Light : register(b0)
{
    float3 lightPos; // ライトのワールド座標
    float dummy; // 16バイトに揃えるためのダミー
    float3 Ldir; // ライトの向き
    float dummy2; // 16バイトに揃えるためのダミー
    float4 Lcolor; // ライトの色
    float4 Lambient; // 環境光
}
cbuffer Camera : register(b1)
{
    float3 Cpos;
    float heightScale;
}

Texture2D tex : register(t0);
SamplerState samp : register(s0);
// 法線マップ
Texture2D normalMap : register(t1);
Texture2D heightMap : register(t2);

float4 main(PS_IN pin) : SV_TARGET
{
    // 高さマップを分割探索する回数を決める
    const int HeightSamples = 32;

    // 法線・接ベクトル・従法線を作り、ワールド空間の視線方向を接空間へ変換する
    float3 N = normalize(pin.normal);
    float3 T = normalize(pin.tangent);
    T = normalize(T - N * dot(N, T));
    float3 B = normalize(cross(T, N));
    float3 V = normalize(Cpos - pin.wPos);
    float3 viewDirTS = normalize(float3(dot(V, T), dot(V, B), dot(V, N)));

    // 接空間の視線方向から、UV上でレイを進める方向と1ステップ分の移動量を作る
    float2 rayDirUV = viewDirTS.xy / max(abs(viewDirTS.z), 0.2f);
    float heightStep = heightScale / HeightSamples;
    float2 uvStep = rayDirUV * heightStep;
    float3 rayStep = float3(-uvStep.x, -heightStep, -uvStep.y);

    // レイ探索の初期位置・現在UV・現在高さを初期化する
    float3 rayPos = float3(pin.uv.x, 0.0f, pin.uv.y);
    float2 uv = pin.uv;
    float rayHeight = rayPos.y;
    float objHeight = -heightScale;

    // レイを1ステップずつ進め、高さマップの高さと交差する位置を探す
    for (int i = 0; i < HeightSamples && rayHeight > objHeight; i++)
    {
        rayPos += rayStep;
        uv = rayPos.xz;
        rayHeight = rayPos.y;

        // 現在のUVで高さマップを読み、レイと比較するための高さに変換する
        float h = heightMap.Sample(samp, uv).r;
        objHeight = h * heightScale - heightScale;
    }

    // 交差した後の点と、1ステップ前の点を取得する
    float2 nextObjPoint = uv;
    float2 prevObjPoint = uv - rayStep.xz;

    // 交差後・交差前それぞれの高さマップ上の高さを取得する
    float nextHeight = objHeight;
    float prevHeight = heightMap.Sample(samp, prevObjPoint).r * heightScale - heightScale;

    // レイ高さとの差分に変換し、どの位置で交差したかを計算できる形にする
    nextHeight -= rayHeight;
    prevHeight -= rayHeight - rayStep.y;

    // 交差前後の差分から線形補間率を計算する
    float denom = nextHeight - prevHeight;
    float weight = 0.0f;

    // 0除算を避けつつ、交差位置の補間率を求める
    if (abs(denom) > 0.00001f)
    {
        weight = saturate(nextHeight / denom);
    }

    // 交差後UVと交差前UVを補間し、より正確なPOM後のUVを求める
    uv = lerp(nextObjPoint, prevObjPoint, weight);
    
    // POMで補正したUVを使ってベーステクスチャを取得する
    float4 color = tex.Sample(samp, uv);

    // 法線マップもPOM後のUVで取得する
    float3 localNormal = normalMap.Sample(samp, uv).xyz;

    // 法線マップの0～1の値を-1～1に変換する
    localNormal = normalize(localNormal * 2.0f - 1.0f);

    // 接空間の法線をワールド空間へ変換する
    float3 finalNormal = normalize(T * localNormal.x + B * localNormal.y + N * localNormal.z);

    // ライト方向を正規化する
    float3 L = normalize(-Ldir);

    // 拡散反射の強さを計算する
    float diffuse = saturate(dot(finalNormal, L));

    // ライト色と環境光を合成する
    float3 lighting = Lcolor.rgb * diffuse + Lambient.rgb;

    // テクスチャ色にライトの色を乗算する
    color.rgb *= lighting;

    // 最終色を返す
    return float4(saturate(color.rgb), color.a);
}