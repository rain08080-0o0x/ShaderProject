struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 tangent : TANGENT;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 wPos : TEXCOORD1;
    float3 tangent : TANGENT;
};

cbuffer ModelMatrix : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
};

VS_OUT main(VS_IN vin)
{
    VS_OUT vout;

    float4 worldPos = mul(world, float4(vin.pos, 1.0f));

    vout.wPos = worldPos.xyz;
    vout.pos = mul(view, worldPos);
    vout.pos = mul(proj, vout.pos);

    float3x3 world3x3 = (float3x3) world;

    vout.normal = normalize(mul(world3x3, vin.normal));
    vout.tangent = normalize(mul(world3x3, vin.tangent));
    vout.uv = vin.uv;

    return vout;
}
