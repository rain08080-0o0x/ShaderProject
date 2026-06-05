#define RIM_LIGHT_MAX 5

struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPos : TEXCOORD1;
};

// C++から渡す全ライト情報。
// float3 + float の詰め間違いを避けるため、全部float4単位で受け取る。
cbuffer AllLightings : register(b0)
{
    // PhongLight
    // xyz = ライト位置, w = 距離減衰範囲
    float4 PhongLightPosRange;
    // rgb = ライト色, a = 拡散反射の強さ
    float4 PhongLightColor;

    // AmbientLight
    // rgb = 環境光色, a = 環境光の強さ
    float4 AmbientLightColor;

    // SpecularLight
    // xyz = ライト位置, w = 鏡面反射の強さ
    float4 SpecularLightPosPower;
    // rgb = 鏡面反射色, a = 光沢の鋭さ
    float4 SpecularLightColorShininess;

    // SpotLight
    // xyz = スポットライト位置, w = 距離減衰範囲
    float4 SpotLightPosRange;
    // xyz = スポットライトの照射方向, w = 角度(degree)
    float4 SpotLightDirAngle;
    // rgb = スポットライト色, a = スポットライトの強さ
    float4 SpotLightColor;

    // RimLight
    // xyz = リムライト方向, w = 未使用
    float4 RimLightDir[RIM_LIGHT_MAX];
    // rgb = リムライト色, a = 個別の強さ
    float4 RimLightColor[RIM_LIGHT_MAX];
    // x = 使用するリムライト数, y = 全体の強さ, z = リムの鋭さ, w = 未使用
    float4 RimLightInfo;
}

cbuffer Camera : register(b1)
{
    float3 Cpos;
    float Cdummy;
}

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float CalcDistanceAttenuation(float3 lightPos, float3 worldPos, float range)
{
    float dist = length(worldPos - lightPos);
    float safeRange = max(range, 0.0001f);
    float atten = 1.0f - dist / safeRange;
    atten = saturate(atten);
    return pow(atten, 3.0f);
}

float3 CalcPhongLight(float3 baseColor, float3 N, float3 worldPos)
{
    float3 L = normalize(PhongLightPosRange.xyz - worldPos);
    float diffuse = saturate(dot(N, L));
    float atten = CalcDistanceAttenuation(PhongLightPosRange.xyz, worldPos, PhongLightPosRange.w);
    float power = PhongLightColor.a;

    return baseColor * PhongLightColor.rgb * diffuse * atten * power;
}

float3 CalcAmbientLight(float3 baseColor)
{
    return baseColor * AmbientLightColor.rgb * AmbientLightColor.a;
}

float3 CalcSpecularLight(float3 N, float3 V, float3 worldPos)
{
    float3 L = normalize(SpecularLightPosPower.xyz - worldPos);
    float diffuseGate = saturate(dot(N, L));

    float3 H = normalize(L + V);
    float shininess = max(SpecularLightColorShininess.a, 1.0f);
    float specular = pow(saturate(dot(N, H)), shininess);

    return SpecularLightColorShininess.rgb * specular * diffuseGate * SpecularLightPosPower.w;
}

float3 CalcSpotLight(float3 baseColor, float3 N, float3 worldPos)
{
    float3 lightToPixel = worldPos - SpotLightPosRange.xyz;
    float3 spotDir = normalize(SpotLightDirAngle.xyz);

    float3 L = normalize(SpotLightPosRange.xyz - worldPos);
    float diffuse = saturate(dot(N, L));
    float atten = CalcDistanceAttenuation(SpotLightPosRange.xyz, worldPos, SpotLightPosRange.w);

    float spotCos = dot(normalize(lightToPixel), spotDir);
    float limitCos = cos(radians(SpotLightDirAngle.w));
    float cone = saturate((spotCos - limitCos) / max(1.0f - limitCos, 0.0001f));
    cone = pow(cone, 0.5f);

    return baseColor * SpotLightColor.rgb * diffuse * atten * cone * SpotLightColor.a;
}

float3 CalcRimLights(float3 N, float3 V)
{
    float3 rimColor = 0.0f;
    int rimCount = min((int)RimLightInfo.x, RIM_LIGHT_MAX);
    float rimPower = RimLightInfo.y;
    float rimSharpness = max(RimLightInfo.z, 0.0001f);

    for (int i = 0; i < rimCount; ++i)
    {
        float3 L = normalize(RimLightDir[i].xyz);

        // カメラから見た輪郭を作る。
        float viewRim = 1.0f - saturate(dot(V, N));
        viewRim = pow(viewRim, rimSharpness);

        // ライト方向も少し反映して、向きごとのリム色差を出す。
        float lightRim = 1.0f - saturate(dot(L, N));

        float strength = RimLightColor[i].a * rimPower;
        rimColor += viewRim * lightRim * RimLightColor[i].rgb * strength;
    }

    return rimColor;
}

float4 main(PS_IN pin) : SV_TARGET
{
    float4 texColor = tex.Sample(samp, pin.uv);

    float3 N = normalize(pin.normal);
    float3 V = normalize(Cpos - pin.wPos);

    float3 baseColor = texColor.rgb;

    float3 ambientLight = CalcAmbientLight(baseColor);
    float3 phongLight = CalcPhongLight(baseColor, N, pin.wPos);
    float3 spotLight = CalcSpotLight(baseColor, N, pin.wPos);
    float3 rimLight = CalcRimLights(N, V);
    float3 specularLight = CalcSpecularLight(N, V, pin.wPos);

    float3 finalColor = ambientLight + phongLight + spotLight + rimLight + specularLight;

    return float4(saturate(finalColor), texColor.a);
}