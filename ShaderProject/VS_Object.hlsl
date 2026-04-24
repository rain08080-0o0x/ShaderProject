struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

cbuffer ModelMatrix : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
};

VS_OUT main( VS_IN vin )
{
    VS_OUT vout;
    vout.pos = float4(vin.pos, 1.0f);
    vout.pos = mul(world, vout.pos);
    vout.pos = mul(view, vout.pos);
    vout.pos = mul(proj, vout.pos);
    vout.uv = vin.uv;
    vout.normal = mul((float3x3)world, vin.normal);
	return vout;
}