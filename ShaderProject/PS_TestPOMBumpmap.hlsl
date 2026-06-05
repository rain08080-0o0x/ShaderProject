// 頂点シェーダーから送られてくるデータの定義
struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0; // 法線
    float3 wPos : TEXCOORD1;
    float3 tangent : TANGENT; // 接ベクトル
};

cbuffer Light : register(b1)
{
    float3 lightPos; // ライトのワールド座標
    float dummy; // 16バイトに揃えるためのダミー
    float3 Ldir; // ライトの向き
    float dummy2; // 16バイトに揃えるためのダミー
    float4 Lcolor; // ライトの色
    float4 Lambient; // 環境光
}
cbuffer Camera : register(b0)
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
    const int HeightSamples = 32;

    float3 N = normalize(pin.normal);
    float3 T = normalize(pin.tangent);
    T = normalize(T - N * dot(N, T));
    float3 B = normalize(cross(T, N));

    float3 V = normalize(Cpos - pin.wPos);
    float3 viewDirTS = normalize(float3(dot(V, T), dot(V, B), dot(V, N)));

    float2 rayDirUV = viewDirTS.xy / max(abs(viewDirTS.z), 0.2f);

    float heightStep = heightScale / HeightSamples;
    float2 uvStep = rayDirUV * heightStep;

    float3 rayStep = float3(-uvStep.x, -heightStep, -uvStep.y);

    float3 rayPos = float3(pin.uv.x, 0.0f, pin.uv.y);
    float2 uv = pin.uv;

    float rayHeight = rayPos.y;
    float objHeight = -heightScale;

    for (int i = 0; i < HeightSamples && rayHeight > objHeight; i++)
    {
        rayPos += rayStep;

        uv = rayPos.xz;
        rayHeight = rayPos.y;

        float h = heightMap.Sample(samp, uv).r;
        objHeight = h * heightScale - heightScale;
    }

    // Parallax Occlusion Mapping
    float2 nextObjPoint = uv;
    float2 prevObjPoint = uv - rayStep.xz;

    float nextHeight = objHeight;
    float prevHeight = heightMap.Sample(samp, prevObjPoint).r * heightScale - heightScale;

    nextHeight -= rayHeight;
    prevHeight -= rayHeight - rayStep.y;

    float denom = nextHeight - prevHeight;
    float weight = 0.0f;

    if (abs(denom) > 0.00001f)
    {
        weight = saturate(nextHeight / denom);
    }

    uv = lerp(nextObjPoint, prevObjPoint, weight);

    return tex.Sample(samp, uv);
}