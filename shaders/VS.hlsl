cbuffer ObjectData : register(b1)
{
    float4x4 worldViewProj;
    float4x4 world;
}

struct VSInput {
    float3 pos : POSITION;
    float3 normal : NORMAL;
};

struct VSOutput {
    float4 pos : SV_POSITION;
    float3 worldNormal : NORMAL;
};

VSOutput main(VSInput input) {
    VSOutput output;
    output.pos = mul(worldViewProj, float4(input.pos, 1.0f));
    output.worldNormal = normalize(mul((float3x3)world, input.normal));
    return output;
}
