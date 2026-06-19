struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPos : TEXCOORD1;
};
cbuffer AD_Param : register(b0)
{
    float4 screenAlphaFade;
    float4 cameraPosParam;
    float4 fadeParam;
};
Texture2D tex : register(t0);
SamplerState samp : register(s0);

float GetBayer4x4(int2 pixelPos)
{
    int x = pixelPos.x & 3;
    int y = pixelPos.y & 3;
    int index = y * 4 + x;

    float bayer[16] =
    {
         0.0f,  8.0f,  2.0f, 10.0f,
        12.0f,  4.0f, 14.0f,  6.0f,
         3.0f, 11.0f,  1.0f,  9.0f,
        15.0f,  7.0f, 13.0f,  5.0f
    };

    return (bayer[index] + 0.5f) / 16.0f;
}

float4 main(PS_IN pin) : SV_TARGET
{
    const float alphaBase = saturate(screenAlphaFade.z);
    float alpha = alphaBase;
    if (screenAlphaFade.w > 0.5f)
    {
        const float dist = distance(cameraPosParam.xyz, pin.wPos);
        const float fadeRange = max(fadeParam.y - fadeParam.x, 0.0001f);
        const float distanceAlpha = saturate((dist - fadeParam.x) / fadeRange);
        alpha *= distanceAlpha;
    }
    const int2 pixelPos = (int2)pin.pos.xy;
    const float threshold = GetBayer4x4(pixelPos);
    if (alpha < threshold)
    {
        discard;
    }
    return tex.Sample(samp, pin.uv);
}
