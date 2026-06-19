// 入力アセンブラ(CPU)から受け取るデータの定義
struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

// 次のラスタライザー・ピクセルシェーダーに送る
// データの定義。頂点シェーダーとピクセルシェーダーで
// データが一致していなくても動くが、一致していないと
// 見た目がおかしくなることがあるので注意！
struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPos : TEXCOORD1;
};
// 表示に必要な変換行列を定数バッファで受け取る
cbuffer ModelMatrix : register(b0)
{
    float4x4 world; // ワールド行列
    float4x4 view; // ビュー行列
    float4x4 proj; // プロジェクション行列
};

VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    // 画面の表示位置の計算
    // mul - 座標のデータに行列の計算を行う関数 DirectX -> XMVectorTransformCood
    vout.pos = float4(vin.pos, 1.0f);
    
    // 輪郭線表示用にモデルを法線方向に膨らませる
    vout.pos.xyz += vin.normal * 0.01f;
    
    vout.pos = mul(world, vout.pos); // ローカル -> ワールド
    vout.wPos = vout.pos.xyz; // ワールド座標のデータを格納
    vout.pos = mul(view, vout.pos); // ワールド -> ビュー
    vout.pos = mul(proj, vout.pos); // ビュー   -> プロジェクション

    // UVは色を付ける(ピクセルシェーダー)処理で必要な
    // ため、頂点シェーダーではただ単に受け渡しだけ行う
    vout.uv = vin.uv;

    // 法線はピクセルシェーダー上で計算する際に
    // ローカル座標系の法線から、ワールド座標系の法線に
    // 変換をかけないとおかしな結果になってしまう
    // また、移動成分を適応してもおかしな結果になるため、
    // 3x3の回転・縮小成分のみの行列に変換してから
    // 計算を行う
    vout.normal = mul((float3x3) world, vin.normal);
    return vout;
}