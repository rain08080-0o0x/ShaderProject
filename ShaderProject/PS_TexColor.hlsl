struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

Texture2D tex : register(t0);
SamplerState samp : register(s0);

// float4 ... 画面に出力する入り情報(r,g,b,a)
float4 main( PS_IN pin ) : SV_TARGET
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    color = tex.Sample(samp, pin.uv); // テクスチャの取得
	return color;    
}