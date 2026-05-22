// 頂点シェーダーから送られてくるデータの定義
struct PS_IN
{
    // 画面上の描画位置
    // ラスタライザーが使う座標なので、基本的にそのまま受け取る
    float4 pos : SV_Position;

    // モデルのテクスチャ座標
    // tex.Sample で画像のどの位置の色を取るかに使う
    float2 uv : TEXCOORD0;

    // ワールド座標系に変換済みの法線
    // 光がどれだけ当たるかを計算するために使う
    float3 normal : NORMAL0;

    // ワールド座標系でのピクセル位置
    // 光源位置からこの位置へ向かう方向を計算するために使う
    float3 wPos : POSITION;
};

// 定数バッファ
// モデルに含まれていないライト情報をC++側から受け取る
// 16バイト単位でデータを送る必要があるため、float3の後ろにdummyを置く
cbuffer Light : register(b0)
{
    // ライトのワールド座標
    // 以前はライトの向きだったが、位置ベースの拡散反射にするため位置として使う
    float3 lightPos;

    // 16バイトに揃えるためのダミー
    float dummy;

    // ライトの色
    // 拡散反射の色として使う
    float4 Lcolor;

    // 環境光
    // 光が直接当たっていない場所にも少し明るさを残すために使う
    float4 Lambient;
}

// モデルに設定されているテクスチャ
Texture2D tex : register(t0);

// テクスチャを読むときのサンプラー
SamplerState samp : register(s0);

float4 main(PS_IN pin) : SV_TARGET
{
    // テクスチャから現在のピクセルの色を取得する
    float4 texColor = tex.Sample(samp, pin.uv);

    // 法線を正規化する
    // 補間や行列変換で長さが1ではなくなることがあるため、計算前に整える
    float3 N = normalize(pin.normal);

    // 光源からではなく、ピクセルから光源へ向かう方向を求める
    // 点光源として扱うため、ライト位置 - ピクセル位置で方向を作る
    float3 L = normalize(lightPos - pin.wPos);

    // 法線とライト方向の内積を計算する
    // 1に近いほど正面から光が当たり、0に近いほど横や裏側になる
    float diffuse = dot(N, L);

    // 裏側に光が回り込まないように、0未満の値を0に丸める
    diffuse = saturate(diffuse);

    // ライト色に拡散反射の強さを掛ける
    // これが光源位置に応じた明るさになる
    float3 diffuseLight = Lcolor.rgb * diffuse;

    // 環境光を少し弱めて加える
    // 直接光が当たらない面も完全な黒にならないようにする
    float3 ambient = Lambient.rgb * 0.3f;

    // 拡散反射と環境光を合わせて、最終的なライティング量にする
    float3 lighting = diffuseLight + ambient;

    // テクスチャ色にライティングを掛ける
    // テクスチャの模様を残したまま、光の当たり方だけ変える
    float3 finalColor = texColor.rgb * lighting;

    // 色を0から1の範囲に収めて返す
    // アルファはテクスチャの値をそのまま使う
    return float4(saturate(finalColor), texColor.a);
}
