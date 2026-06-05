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
    // 1.テクスチャから法線の情報を取得する
    float3 localNormal = normalMap.Sample(samp, pin.uv).xyz;
    
    // 色情報(0~1)として保存されている為、
    // ベクトル情報(-1~1)に変換しなおす
    localNormal = (localNormal - 0.5f) * 2.0f;
    
    // 2.テクスチャの法線はタンジェントスペースのため
    //   ワールドスペースに変換する
    //-- 直行行列
    // X軸,Y軸,Z軸のいずれも90度で直行している行列
    // 行列の各成分には行列で変換した後の
    // ワールド空間上の各軸の向きを表す情報を持つ
    // | 1 0 0 0 | ← 1行目の先頭3つにX軸
    // | 0 1 0 0 | ← 2行目の先頭3つにY軸
    // | 0 0 1 0 | ← 3行目の先頭3つにZ軸
    // | 0 0 0 1 | ← 
    // 返還後のベクトルの向きを行列の各成分に当て込む
    // ことで、変換行列を作成することができる
    
    // 2-1.従法線ベクトルの計算
    float3 biNormal = normalize(cross(pin.normal,pin.tangent));
    
    // 2-2.タンジェントからワールドへの行列変換
    //         | Tx Ty Tz |
    // [x,y,z] | Bx By Bz |
    //         | Nx Ny Nz |
    // 下の計算は、上の行列計算と同等(P.164
    float3 N =
        pin.tangent * localNormal.x +
        biNormal    * localNormal.y +
        pin.normal  * localNormal.z;
    
    N = normalize(N);
    
    // 3.変換された法線とライトで拡散反射を計算
    float3 L = normalize(float3(Ldir.x,Ldir.y,-Ldir.z));
    float d = dot(N, L);
    d = saturate(-d);
    
    float4 color = tex.Sample(samp, pin.uv);
    color.rgb = color.rgb * Lcolor.rgb * d;
    
    return color;

}
