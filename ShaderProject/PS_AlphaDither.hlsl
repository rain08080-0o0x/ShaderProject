struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPos : TEXCOORD1; // サーフェイルの光が当たった場所
    float3 sPos : POSITION1;
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
    float Cdummy1;
}

cbuffer AD_Param : register(b2)
{
    float2 screenSize;
    float2 ADdummy0;
    float3 cameraPos;
    float ADdummy1;
}


Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(PS_IN pin) : SV_TARGET
{
    // お試しでUVの値に応じて穴を開ける
    float val1 = (pin.uv.x + pin.uv.y) * 100.0f, valX = pin.uv.x * 100.0f, valY = pin.uv.y * 100.0f;

    if (/*frac(val1) < 0.5f ||*/ (frac(valX) < 0.5f || frac(valY) < 0.5f) )
    {
        if (!(frac(valX) < 0.5f && frac(valY) < 0.5f))
            discard;
    }
    // 該当の閾値
    float thershold;


    float2 sPos = pin.sPos.xy * 0.5f +0.5f;
    
    sPos *= float2(960,540);
    
    
    // カメラとの距離に応じてディザ抜きを判定
    // する位置を計算
    float camLen = length(pin.wPos - cameraPos);
    
    float ditherNClip = 1.0f; // ディザ抜き最低距離
    float ditherFClip = 3.0f; // ディザ抜き限界距離
    
    float val = (camLen - ditherNClip) / (ditherFClip - ditherNClip);
    
    if(val < thershold)
    {
        discard;
    }
    
    // 
    float4 color = tex.Sample(samp, pin.uv);
    return color;
}