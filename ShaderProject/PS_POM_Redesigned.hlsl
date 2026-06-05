struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPos : TEXCOORD1;
    float3 tangent : TANGENT;
};

cbuffer Light : register(b0)
{
    float3 lightPos;
    float dummy;
    float3 Ldir;
    float dummy2;
    float4 Lcolor;
    float4 Lambient;
};

cbuffer Camera : register(b1)
{
    float3 Cpos;
    float heightScale;
};

cbuffer POMSetting : register(b2)
{
    float minLayerCount;
    float maxLayerCount;
    float uvDiscard;
    float heightBias;
};

Texture2D tex : register(t0);
Texture2D normalMap : register(t1);
Texture2D heightMap : register(t2);
SamplerState samp : register(s0);

float2 ParallaxOcclusionMapping(float2 uv, float3 viewDirTS)
{
    viewDirTS = normalize(viewDirTS);

    float viewZ = max(abs(viewDirTS.z), 0.001f);
    float layerRate = saturate(viewZ);

    int layerCount = (int) lerp(maxLayerCount, minLayerCount, layerRate);
    layerCount = clamp(layerCount, minLayerCount, maxLayerCount);

    float layerDepth = 1.0f / (float) layerCount;
    float currentLayerDepth = 0.0f;

    float2 deltaUV = (viewDirTS.xy / viewZ) * heightScale / (float) layerCount;

    float2 currentUV = uv;
    float2 prevUV = uv;

    float currentHeight = saturate(1.0f - heightMap.SampleLevel(samp, currentUV, 0.0f).r + heightBias);

    bool hit = false;

    [unroll(32)]
    for (int i = 0; i < 32; ++i)
    {
        bool active = (i < layerCount) && !hit;

        if (active)
        {
            prevUV = currentUV;

            currentUV -= deltaUV;
            currentLayerDepth += layerDepth;
            currentHeight = saturate(1.0f - heightMap.SampleLevel(samp, currentUV, 0.0f).r + heightBias);

            hit = currentLayerDepth >= currentHeight;
        }
    }

    float afterDepth = currentHeight - currentLayerDepth;

    float beforeHeight = saturate(1.0f - heightMap.SampleLevel(samp, prevUV, 0.0f).r + heightBias);
    float beforeDepth = beforeHeight - currentLayerDepth + layerDepth;

    float denominator = afterDepth - beforeDepth;
    float weight = 0.0f;

    if (abs(denominator) > 0.00001f)
    {
        weight = saturate(afterDepth / denominator);
    }

    return lerp(currentUV, prevUV, weight);
}

float4 main(PS_IN pin) : SV_TARGET
{
    float3 N0 = normalize(pin.normal);
    float3 T = normalize(pin.tangent);
    T = normalize(T - N0 * dot(N0, T));
    float3 B = normalize(cross(N0, T));

    float3 V = normalize(Cpos - pin.wPos);
    float3 viewDirTS = float3(dot(V, T), dot(V, B), dot(V, N0));

    float2 uv = ParallaxOcclusionMapping(pin.uv, viewDirTS);

    if (uvDiscard > 0.5f)
    {
        if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f)
        {
            discard;
        }
    }

    float3 localNormal = normalMap.Sample(samp, uv).xyz;
    localNormal = normalize(localNormal * 2.0f - 1.0f);

    float3 N = normalize(T * localNormal.x + B * localNormal.y + N0 * localNormal.z);
    float3 L = normalize(-float3(Ldir.x, Ldir.y, -Ldir.z));
    float diffuse = saturate(dot(N, L));

    float4 color = tex.Sample(samp, uv);
    float3 lighting = Lcolor.rgb * diffuse + Lambient.rgb;
    color.rgb *= lighting;

    return float4(saturate(color.rgb), color.a);
}
