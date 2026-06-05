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
}

cbuffer Camera : register(b1)
{
    float3 Cpos;
    float heightScale;
}

Texture2D tex : register(t0);
SamplerState samp : register(s0);
Texture2D normalMap : register(t1);
Texture2D heightMap : register(t2);

float SampleHeight(float2 uv)
{
    // 高さの向きを補正して取得
    return 1.0f - heightMap.SampleLevel(samp, uv, 0.0f).r;
}

float2 ParallaxOcclusionMapping(float2 uv, float3 viewDirTS)
{
    const int HeightSamples = 48;
    float height = clamp(heightScale, 0.0f, 0.08f);
    float3 rayDir = normalize(-viewDirTS);
    rayDir.z = min(rayDir.z, -0.15f);

    float rayScale = -height / rayDir.z;
    float3 rayStep = rayDir * rayScale / HeightSamples;
    float2 currentUV = uv;
    float rayHeight = 0.0f;
    float objHeight = SampleHeight(currentUV) * height - height;

    [loop]
    for (int i = 0; i < HeightSamples; ++i)
    {
        if (objHeight >= rayHeight)
        {
            break;
        }

        currentUV += rayStep.xy;
        rayHeight += rayStep.z;
        objHeight = SampleHeight(currentUV) * height - height;
    }

    float2 nextUV = currentUV;
    float2 prevUV = currentUV - rayStep.xy;
    float nextRayHeight = rayHeight;
    float prevRayHeight = rayHeight - rayStep.z;
    float nextObjHeight = objHeight;
    float prevObjHeight = SampleHeight(prevUV) * height - height;
    float nextDiff = nextObjHeight - nextRayHeight;
    float prevDiff = prevObjHeight - prevRayHeight;
    float denom = nextDiff - prevDiff;
    float safeDenom = (abs(denom) < 0.0001f) ? (denom < 0.0f ? -0.0001f : 0.0001f) : denom;
    float weight = saturate(nextDiff / safeDenom);

    return lerp(nextUV, prevUV, weight);
}

float4 main(PS_IN pin) : SV_TARGET
{
    float3 N0 = normalize(pin.normal);
    float3 T = normalize(pin.tangent);
    T = normalize(T - N0 * dot(N0, T));
    float3 B = normalize(cross(T, N0));

    float3 V = normalize(Cpos - pin.wPos);
    float3 viewDirTS = normalize(float3(dot(V, T), dot(V, B), dot(V, N0)));

    float2 uv = ParallaxOcclusionMapping(pin.uv, viewDirTS);

    float3 localNormal = normalMap.Sample(samp, uv).xyz;
    localNormal = localNormal * 2.0f - 1.0f;

    float3 N = normalize(T * localNormal.x + B * localNormal.y + N0 * localNormal.z);
    float3 L = normalize(-Ldir);
    float diffuse = saturate(dot(N, L));

    float4 color = tex.Sample(samp, uv);
    float3 lighting = Lcolor.rgb * diffuse + Lambient.rgb;
    color.rgb *= lighting;

    return float4(saturate(color.rgb), color.a);
}
